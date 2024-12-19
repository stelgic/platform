#include <g3log/g3log.hpp>
#include <datamodels/OrderData.h>
#include "CrossEmaStrategy.h"

namespace stelgic
{
using namespace g3;

void CrossEmaStrategy::InitStateMap()
{
    // initialize state map
    SetState("validated", false);
    for(const auto& instrum: appPtr->GetActivePortfolio().instruments)
    {
        int64_t crossIndex = -1;
        SetState(instrum, StateMap{});

        // initialize states
        auto& state = std::any_cast<StateMap&>(multiState.at(instrum));
        state.insert_or_assign("upcross", false);
        state.insert_or_assign("downcross", false);
        state.insert_or_assign("overbought", false);
        state.insert_or_assign("oversold", false);
        state.insert_or_assign("overboughtend", false);
        state.insert_or_assign("oversoldend", false);
        state.insert_or_assign("long", false);
        state.insert_or_assign("short", false);
        state.insert_or_assign("crossIndex", crossIndex);
    }
}

bool CrossEmaStrategy::ValidateInputs(const StrategyInputData& input)
{
    int64_t epoch = input.timestamp;
    const size_t& barsCount = input.batchSize;
    const size_t& barsFieldCount = input.perBarsFieldCount;
    const std::set<std::string>& instruments = input.instruments; 
    std::shared_ptr<arrow::RecordBatch> ohlcvRecord = input.ohlcvRecord;
    const IndicatorList& indicators = input.indicators;

    // check all require params is available and return true or false
    for(int i=0; i < indicators.size(); ++i)
    {
        if(indicators.at(i).label == "rsi")
            rsiIdx = i;
        else if(indicators.at(i).label == "atr")
            atrIdx = i;
        else if(indicators.at(i).label == "shortema")
            shortEmaIdx = i;
        else if(indicators.at(i).label == "midema")
            midEmaIdx = i;
        else if(indicators.at(i).label == "longema")
            longEmaIdx = i;
    }

    if(rsiIdx == -1 || atrIdx == -1 || shortEmaIdx == -1 || longEmaIdx == -1 || midEmaIdx == -1)
        return false;

    return true;
}

void CrossEmaStrategy::Evaluate(const StrategyInputData& input)
{
    if (exitThread) return;

    // validate input data for strategy once
    if(std::any_cast<bool>(multiState.at("validated")) == false)
    {
        bool validated = ValidateInputs(input);
        SetState("validated", validated);
        if(!validated)
        {
            LOG_IF(WARNING, verboseLevel > 0) << "Strategy input data invalid";
            return; 
        }
    }
    
    auto signals = PredictSignal(input);
    for(const auto& signal: signals)
    {
        //OrderData order = tradeManager->CreateOrder(signal);
        //tradeManager->InsertOrUpdateOrder(order);
        //LOG(WARNING) << order.toJson();

        std::string instrum = signal["instrum"].asString();
        bool closing = signal.get("closeposition", false).asBool();
        bool hasPosition = tradeManager->HasPosition(instrum);
        if(!hasPosition && !closing) // check if position open for instrum
        {
            PositionData posData = tradeManager->BuildNewPosition(signal);
            if(posData.IsValid())
                tradeManager->InsertOrUpdatePosition(posData);
        }
        else if(hasPosition && closing)
        {
            PositionData posData = tradeManager->BuildNewPosition(signal);
            posData = tradeManager->ExitPosition(posData, 0.0, true);
            if(posData.IsValid())
                tradeManager->UpdateTradeHistoryTable({posData});
        }
    }
    
    const std::set<std::string>& instruments = input.instruments; 
    LOG_IF(INFO, verboseLevel > 0) << "Evaluating strategy for " 
                    << instruments.size() << " instruments";
}

std::vector<Json::Value> CrossEmaStrategy::PredictSignal(const StrategyInputData& input)
{
    std::vector<Json::Value> tradeSignals;
    
    // get reference to data
    const int64_t& rowIndex = input.rowIndex;
    const size_t& barsCount = input.batchSize;
    const size_t& barsFieldCount = input.perBarsFieldCount;
    const std::string& sourceName = input.sourceName;
    const std::string& assetClass = input.assetClass;
    const std::set<std::string>& instruments = input.instruments; 
    const IndicatorList& indicators = input.indicators;
    std::shared_ptr<arrow::Table> ohlcvChunk = input.ohlcvChunkTable;
    std::shared_ptr<arrow::Table> processedChunk = input.processedChunkTable;

    for(const auto& instrum: instruments)
    {
        Json::Value tradeSignal;   
        
        const auto& rsi = indicators.at(rsiIdx);
        const auto& atr = indicators.at(atrIdx);
        const auto& shortEma = indicators.at(shortEmaIdx);
        const auto& midEma = indicators.at(midEmaIdx);
        const auto& longEma = indicators.at(longEmaIdx);

        // construct column name
        auto rsiNames = BuildProcessorsColumnNames(instrum, rsi, "label");
        auto atrNames = BuildProcessorsColumnNames(instrum, atr, "label");
        auto shortEmaNames = BuildProcessorsColumnNames(instrum, shortEma, "label");
        auto midEmaNames = BuildProcessorsColumnNames(instrum, midEma, "label");
        auto longEmaNames = BuildProcessorsColumnNames(instrum, longEma, "label");
        std::string closeName = instrum;
        closeName.append("#").append("close");

        // cast arrow array scalar to raw value
        const auto* timesData = castTableColumn<int64_t>(ohlcvChunk->GetColumnByName("timestamp"));
        const auto* closeData = castTableColumn<double>(ohlcvChunk->GetColumnByName(closeName));
        const auto* rsiData = castTableColumn<double>(processedChunk->GetColumnByName(rsiNames.at(0)));
        const auto* atrData = castTableColumn<double>(processedChunk->GetColumnByName(atrNames.at(0)));
        const auto* shortEmaData = castTableColumn<double>(processedChunk->GetColumnByName(shortEmaNames.at(0)));
        const auto* midEmaData = castTableColumn<double>(processedChunk->GetColumnByName(midEmaNames.at(0)));
        const auto* longEmaData = castTableColumn<double>(processedChunk->GetColumnByName(longEmaNames.at(0)));

        if(!rsiData || !atrData || !shortEmaData || !midEmaData || !longEmaData)
        {
            LOG_IF(WARNING, verboseLevel > 0) 
                << "Failed to get processed data [" 
                << rsiNames.at(0) << "," << rsiNames.at(0) << "," << shortEmaNames.at(0) << "," 
                << midEmaNames.at(0) << "," << longEmaNames.at(0) << "]";
            return tradeSignals;
        }

        auto& state = std::any_cast<StateMap&>(GetState(instrum));

        // compute RSI simple moving average   
        size_t smaWinsize = 7;
        int64_t currIndex = rowIndex;
        int64_t backIndex = currIndex - smaWinsize;
        int64_t crossIndex = std::get<int64_t>(state.at("crossIndex"));
        crossIndex = (crossIndex == -1) ? currIndex: crossIndex;

        double rsiSma = std::accumulate(rsiData + backIndex, rsiData + currIndex, 0.0) / smaWinsize;
        double shortCrossPerc = (shortEmaData[currIndex] - longEmaData[currIndex]) / longEmaData[currIndex] * 100;  
        double midCrossPerc = (shortEmaData[currIndex] - midEmaData[currIndex]) / midEmaData[currIndex] * 100;        
        double longPerc = (longEmaData[currIndex] - longEmaData[crossIndex]) / longEmaData[crossIndex] * 100;

        if((shortEmaData[backIndex] < longEmaData[backIndex]) &&
            shortEmaData[currIndex] > longEmaData[currIndex] && 
            shortCrossPerc > 0.02 && midCrossPerc > 0.02)
        {
            state.insert_or_assign("upcross", true);
            state.insert_or_assign("downcross", false);
            state.insert_or_assign("crossIndex", currIndex);
        }
        else if((shortEmaData[backIndex] > longEmaData[backIndex]) &&
            shortEmaData[currIndex] < longEmaData[currIndex] && shortCrossPerc < -0.02)
        {
            int64_t resetIndex = -1;
            state.insert_or_assign("downcross", true);
            state.insert_or_assign("upcross", false);
            state.insert_or_assign("crossIndex", resetIndex);
        }

        bool upcross = std::get<bool>(state.at("upcross"));
        bool downcross = std::get<bool>(state.at("downcross"));

        // normalized ATR above 0.17 overbought start
        if(!std::get<bool>(state.at("overbought")) && atrData[currIndex] > 0.13 && upcross)
        {
            state.insert_or_assign("overbought", true);
            state.insert_or_assign("overboughtend", false);
        }
        // rsi less 67 overbought ends
        else if(std::get<bool>(state.at("overbought")) && rsiSma < 66)
        {
            state.insert_or_assign("overbought", false);
            state.insert_or_assign("overboughtend", true);
        }
        // rsi below 30 overbought start
        else if(!std::get<bool>(state.at("oversold")) && rsiSma < 27 && downcross)
        {
            state.insert_or_assign("oversold", true);
            state.insert_or_assign("oversoldend", false);
        }
        // rsi above 33 overbought ends
        else if(std::get<bool>(state.at("oversold")) && rsiSma > 32)
        {
            state.insert_or_assign("oversold", false);
            state.insert_or_assign("oversoldend", true);
        }

        // short EMA cross above long EMA, take long position
        if(!std::get<bool>(state.at("long")) && upcross && 
            shortCrossPerc > 0.03 && midCrossPerc > 0.02)
        {        
            tradeSignal["side"] = "BUY";
            tradeSignal["posside"] = "LONG";
            tradeSignal["closeposition"] = false;
            tradeSignal["instrum"] = instrum;
            tradeSignal["assetclass"] = assetClass;
            tradeSignal["source"] = sourceName;
            tradeSignal["price"] = closeData[currIndex];
            tradeSignal["quantity"] = GetQuantity(closeData[currIndex]);
            tradeSignal["ltimestamp"] = timesData[currIndex];
            tradeSignal["strategy"] = std::string(moduleName);

            tradeSignals.push_back(tradeSignal);
            state.insert_or_assign("long", true);
            state.insert_or_assign("oversoldend", false);
            state.insert_or_assign("upcross", false);
        }
        else
        {
            // rsi below 57 overbought ends, exit position
            bool overboughtend = std::get<bool>(state.at("overboughtend"));
            if(std::get<bool>(state.at("long")) && 
                (shortCrossPerc < -0.07 || overboughtend))
            {
                tradeSignal["side"] = "SELL";
                tradeSignal["posside"] = "LONG";
                tradeSignal["closeposition"] = true;
                tradeSignal["instrum"] = instrum;
                tradeSignal["assetclass"] = assetClass;
                tradeSignal["source"] = sourceName;
                tradeSignal["price"] = closeData[currIndex];
                tradeSignal["ltimestamp"] = timesData[currIndex];
                tradeSignal["strategy"] = std::string(moduleName);

                tradeSignals.push_back(tradeSignal);
                state.insert_or_assign("long", false);
                state.insert_or_assign("downcross", false);
                state.insert_or_assign("oversoldend", false);
                state.insert_or_assign("overboughtend", false);
            }
        }
    }

    return tradeSignals;
}
}

STRATEGY_MODULE(stelgic::CrossEmaStrategy, stelgic::CrossEmaStrategy::moduleName, "0.0.1");


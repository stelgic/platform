// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#include "SimpleStrategy.h"
#include <g3log/g3log.hpp>

namespace stelgic
{
using namespace g3;

void SimpleStrategy::InitStateMap()
{
    // initialize state map
    SetState("validated", false);
    for(const auto& instrum: appPtr->GetActivePortfolio().instruments)
    {
        SetState(instrum, StateMap{});

        // initialize states
        auto& state = std::any_cast<StateMap&>(GetState(instrum));

    }   
}

bool SimpleStrategy::ValidateInputs(const StrategyInputData& input)
{
    int64_t epoch = input.timestamp;
    const size_t& barsCount = input.batchSize;
    const size_t& barsFieldCount = input.perBarsFieldCount;
    const std::set<std::string>& instruments = input.instruments; 
    std::shared_ptr<arrow::RecordBatch> ohlcvRecord = input.ohlcvRecord;
    const IndicatorList& indicators = input.indicators;

    // check all require params is available and return true or false

    return true;
}

void SimpleStrategy::Evaluate(const StrategyInputData& input)
{
    if (exitThread) return;

    // validate input data for strategy once
    if(std::any_cast<bool>(multiState.at("validated")) == false)
    {
        bool validated = ValidateInputs(input);
        multiState.insert_or_assign("validated", validated);
        if(!validated)
        {
            LOG_IF(WARNING, verboseLevel > 0) << "Strategy input data invalid";
            return; 
        }
    }

    const std::set<std::string>& instruments = input.instruments; 
    
    /* TODO: predict buy sell signal
    * Make call to ML or any trading model
    */

    /* TODO: create order and trades
    * Json::Value params;
    * OrderData order = tradeManager->BuildNewOrder(params);
    * OrderData order = tradeManager->CreateOrder(params);
    */

    LOG_IF(INFO, verboseLevel > 0) << "Evaluating strategy for " 
                    << instruments.size() << " instruments";
}

std::vector<Json::Value> SimpleStrategy::PredictSignal(const StrategyInputData& input)
{
    std::vector<Json::Value> tradeSignalDatas;

    // TODO: implement trade strategy or signal prediction

    return tradeSignalDatas;
}
}

STRATEGY_MODULE(stelgic::SimpleStrategy, stelgic::SimpleStrategy::moduleName, "0.0.1");


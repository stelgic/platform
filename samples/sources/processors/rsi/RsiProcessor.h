#pragma once

#include <IDataProcessor.h>

using namespace stelgic;

/**
 * cache last computed values for each instrument 
 * to be use on next calculation
 */
struct RSI
{
    RSI() {}
    virtual ~RSI() {}
    RSI(size_t len, double initValue) { 
        upAvgs.resize(len, initValue); 
        downAvgs.resize(len, initValue); 
        prevRsis.resize(len, initValue); 
        prevPrices.resize(len, initValue); 
    }

    std::vector<double> upAvgs;
    std::vector<double> downAvgs;
    std::vector<double> prevRsis;
    std::vector<double> prevPrices;
};

/**
 * Implements RSI technical indicator processor module
 */
class RsiProcessor : public IDataProcessor
{
public:
    using Tag = IndicTag<RSI>;
    static constexpr const char* moduleName = "rsi";

public:
    RsiProcessor() {}
    virtual ~RsiProcessor() {}

    virtual TaskArray Evaluate(
        StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
        const double &errorVal, bool& allocate, std::atomic_bool& success) override;

    std::vector<std::string> const& GetOutNames() const override 
    {
        return outNames;
    }

    DataTag GetDataTag() override { return DataTags::OHLCV(); }

protected:
    std::vector<std::string> outNames = {"RSI"};
};



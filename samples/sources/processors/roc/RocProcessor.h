#pragma once

#include <IDataProcessor.h>

using namespace stelgic;

/**
 * cache last winsize prices for each instrument 
 * to be use on next calculation
 */
struct ROC 
{
    ROC() {}
    virtual ~ROC() {}
    ROC(size_t len, int winSize, double initValue) { 
        values.resize(len, DoubleQueue(winSize, initValue)); 
    }
    QueueVector values;
};

/**
 * Implements ROC technical indicator processor module
 */
class RocProcessor : public IDataProcessor
{
public:
    using Tag = IndicTag<ROC>;
    static constexpr const char* moduleName = "roc";

public:
    RocProcessor() {}
    virtual ~RocProcessor() {}

    virtual TaskArray Evaluate(
        StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
        const double &errorVal, bool& allocate, std::atomic_bool& success) override;
    
    std::vector<std::string> const& GetOutNames() const override 
    {
        return outNames;
    }

    DataTag GetDataTag() override { return DataTags::OHLCV(); }

protected:
    std::vector<std::string> outNames = {"ROC"};
};



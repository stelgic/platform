// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#pragma once

#include <IDataProcessor.h>

using namespace stelgic;

/**
 * cache last computed value for each instrument 
 * to be use on next calculation
 */
struct EMA {
    EMA() {}
    virtual ~EMA() {}
    EMA(size_t len, double initValue) { values.resize(len, initValue); }
    DoubleVector values;
};

/**
 * Implements EMA technical indicator processor module
 */
class EmaProcessor : public IDataProcessor
{
public:
    using Tag = IndicTag<EMA>;
    static constexpr const char* moduleName = "ema";

public:

    EmaProcessor() {}
    virtual ~EmaProcessor() {}
    
    virtual TaskArray Evaluate(
        StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
        const double &errorVal, bool& allocate, std::atomic_bool& success) override;

    std::vector<std::string> const& GetOutNames() const override 
    {
        return outNames;
    }

    DataTag GetDataTag() override { return DataTags::OHLCV(); }

protected:
    std::vector<std::string> outNames = {"EMA"};
};



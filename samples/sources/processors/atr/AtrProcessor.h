// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#pragma once

#include <IDataProcessor.h>

using namespace stelgic;

/**
 * cache last computed value for each instrument 
 * to be use on next calculation
 */
struct ATR 
{
    ATR() {}
    virtual ~ATR() {}
    ATR(size_t len, double initValue) 
    { 
        prevAtrs.resize(len, initValue); 
        prevCloses.resize(len, initValue); 
    }
    
    DoubleVector prevAtrs;
    DoubleVector prevCloses;
};

/**
 * Implements ATR Indicator processor module
 */
class AtrProcessor : public IDataProcessor
{
public:
    using Tag = IndicTag<ATR>;
    static constexpr const char* moduleName = "atr";

public:
    AtrProcessor() {}
    virtual ~AtrProcessor() {}

    virtual TaskArray Evaluate(
        StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
        const double &errorVal, bool& allocate, std::atomic_bool& success) override;

    std::vector<std::string> const& GetOutNames() const override 
    {
        return outNames;
    }

    DataTag GetDataTag() override { return DataTags::OHLCV(); }

protected:
    std::vector<std::string> outNames = {"ATR"};
};


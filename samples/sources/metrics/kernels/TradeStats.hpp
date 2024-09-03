// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#include <IMetricKernel.h>

namespace stelgic
{
struct TradeStats : IMetricKernel
{
    int64_t numTrades = 0;
    int64_t winTrades = 0;
    double lossPerc = 0;

    void reset() override 
    { 
        numTrades = 0; 
        winTrades = 0;
        lossPerc = 0.0;
    }

    void update(const double* values, int64_t index, 
        int64_t oindex, void* outValues=nullptr) override
    {
        ++numTrades;
        if(values[index] > 0.0)
            ++winTrades;
    }

    OutType GetType() override
    {
        return OutType::JSON;
    }
    
    Json::Value GetValue() override
    {
        Json::Value data;
        lossPerc = (numTrades > 0) ? 100.0 * (numTrades - winTrades) / numTrades : 0;
        data["numTrades"] = numTrades;
        data["winTrades"] = winTrades;
        data["lossTrades"] = numTrades - winTrades;
        data["winPerc"] = (numTrades > 0)? 100.0 - lossPerc: 0.0;
        data["lossPerc"] = lossPerc;
        data["sharpe"] = 0.0;
        data["sortino"] = 0.0;
        data["sortinov2"] = 0.0;
        data["calmar"] = 0.0;
        data["volatility"] = 0.0;

        return data;
    }

    uint8_t ColorCode() override
    {
        uint8_t color = 0;
        return color;
    }
};

auto tradestats = REGISTER_KERNEL(IMetricKernel::kernelRegistry, "tradestats", TradeStats);
}


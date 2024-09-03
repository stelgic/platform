// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#include <IMetricKernel.h>

namespace stelgic
{
struct MaxDrawdown : IMetricKernel
{
    double cumulativeSum = 0.0;
    double peekSum = 0.0;
    double maxdrawdown = 0.0;

    void reset() override 
    { 
        cumulativeSum = 0.0;
        maxdrawdown = 0.0; 
        peekSum = 0.0;
    }

    void update(const double* values, int64_t index, 
        int64_t oindex, void* outValues=nullptr) override
    {
        cumulativeSum += values[index];
        peekSum = std::max(peekSum, cumulativeSum);
        double drawdown = std::min(0.0, (cumulativeSum - peekSum));
        maxdrawdown = std::min(maxdrawdown, drawdown);
        if(outValues) 
            static_cast<double*>(outValues)[oindex] = drawdown;
    }

    OutType GetType() override
    {
        return OutType::COLUMN;
    }

    Json::Value GetValue() override
    {
        return std::min(0.0,maxdrawdown);
    }

    uint8_t ColorCode() override
    {
        uint8_t color = 0;
        if(maxdrawdown > 0)
            color = 1;
        else if(maxdrawdown < 0)
            color = 2;
        return color;
    }
};

auto maxdrawdown = REGISTER_KERNEL(IMetricKernel::kernelRegistry, "maxdrawdown", MaxDrawdown);
}


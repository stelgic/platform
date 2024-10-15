#include <IMetricKernel.h>

namespace stelgic
{
struct CumulReturn : IMetricKernel
{
    double cumulativeSum = 0.0;
    void Reset() override 
    { 
        cumulativeSum = 0.0; 
    }

    void Update(const double* values, int64_t index, 
        int64_t oindex, void* outValues=nullptr) override
    {
        cumulativeSum += values[index];
        if(outValues) 
            static_cast<double*>(outValues)[oindex] = cumulativeSum;
    }

    OutType GetType() override
    {
        return OutType::COLUMN;
    }

    Json::Value GetValue() override
    {
        return cumulativeSum;
    }

    uint8_t ColorCode() override
    {
        uint8_t color = 0;
        if(cumulativeSum > 0)
            color = 1;
        else if(cumulativeSum < 0)
            color = 2;
        return color;
    }
};

auto cumulreturn = REGISTER_KERNEL(IMetricKernel::kernelRegistry, "cumulreturn", CumulReturn);
}



// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#include "RsiKernel.h"
#include "RsiProcessor.h"
#include <TableColumnHelper.h>

TaskArray RsiProcessor::Evaluate(
    StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
    const double &errorVal, bool& allocate, std::atomic_bool& success)
{
    TaskArray tasks;
    
    tasks.push_back(std::thread([&](){
        const size_t symbolsCount = data.attrs["ninstruments"].asInt64();
        const int nfields = data.attrs["nfields"].asInt();
        const int field = data.attrs["field"].asInt();
        const size_t numSamples = data.attrs["nsamples"].asInt64();
        const int winSize = data.attrs["winsize"].asInt();
        const double scale = data.attrs.get("scale", 100.0).asDouble();

        if(allocate || !data.cachedValues.has_value())
            data.cachedValues.emplace<RSI>(symbolsCount, errorVal);
        
        RSI& caches = std::any_cast<RSI&>(data.cachedValues);
        double* outdata = const_cast<double*>(
            std::static_pointer_cast<arrow::DoubleArray>(data.outdata)->raw_values());

        TaskArray subtasks;
        std::atomic<int> counter = {0};
        std::vector<int> colIds(nfields); // ohlcv
        
        bool validated = (dataRef.ohlcvRecord) ? dataRef.ohlcvRecord->Validate().ok(): false;
        if(outdata && caches.upAvgs.data() && caches.downAvgs.data() && validated)
        {
            for(size_t id=0; id < symbolsCount; ++id)
            {
                size_t j = id * nfields + 1; // add 1 to skip first column timestamp
                //std::iota(std::begin(colIds), std::end(colIds), j);
                dataRef.lock.Lock();
                auto fieldColumn = dataRef.ohlcvRecord->column(j+field);
                dataRef.lock.Unlock();
                if(fieldColumn && outdata)
                {
                    const double* fieldValues = castTableColumn<double>(fieldColumn);
                    bool result = RelativeStrengthIndex(
                        id, fieldValues, numSamples, aSize, outdata, winSize, field, 
                        nfields, &errorVal, caches.upAvgs.data(), caches.downAvgs.data(), 
                        caches.prevRsis.data(), caches.prevPrices.data(), scale);
                    counter += (int)result;
                }
            }
        }
        
        if(success && counter != (int)symbolsCount)
            success.store(false);

        subtasks.resize(0);
    }));

    return tasks;
}

PROCESSOR_MODULE(RsiProcessor, RSI, RsiProcessor::moduleName, "0.0.1");

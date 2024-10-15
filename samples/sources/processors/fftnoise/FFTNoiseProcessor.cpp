#include "FFTNoiseKernel.h"
#include "FFTNoiseProcessor.h"
#include <TableColumnHelper.h>

TaskArray FFTNoiseProcessor::Evaluate(
    StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
    const double &errorVal, bool& allocate, std::atomic_bool& success)
{
    TaskArray tasks;
    
    tasks.push_back(std::thread([&](){
        const size_t symbolsCount = data.attrs["ninstruments"].asInt64();
        const int nfields = data.attrs["nfields"].asInt();
        const int field = data.attrs["field"].asInt();
        const size_t numSamples = data.attrs["nsamples"].asInt64();
        const int cutOff = data.attrs["cutoff"].asInt();
        
        if(allocate || !data.cachedValues.has_value())
        {
            FFTNOISE fftCache;
            FFTNOISE& cached = data.cachedValues.emplace<FFTNOISE>(fftCache); 
            // initialize FFT input and output data array
            dataRef.lock.Lock();
            cached.init(numSamples);
            dataRef.lock.Unlock();
        }
        
        FFTNOISE& fftCache = std::any_cast<FFTNOISE&>(data.cachedValues);
        double* outdata = const_cast<double*>(
            std::static_pointer_cast<arrow::DoubleArray>(data.outdata)->raw_values());

        TaskArray subtasks;
        std::atomic<int> counter = {0};
        std::vector<int> colIds(nfields); // ohlcv

        bool validated = (dataRef.ohlcvRecord) ? dataRef.ohlcvRecord->Validate().ok(): false;
        if(outdata && fftCache.in_plan && fftCache.inputFreq.data() && validated)
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
                    bool result = FFTNoiseFilter(id, fieldValues, 
                        numSamples, aSize, fftCache.in_plan, fftCache.out_plan,
                        reinterpret_cast<fftw_complex*>(fftCache.inputFreq.data()),
                        reinterpret_cast<fftw_complex*>(fftCache.outFreq.data()), 
                        outdata, cutOff, field, nfields, &errorVal);
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

PROCESSOR_MODULE(FFTNoiseProcessor, FFTNOISE,FFTNoiseProcessor::moduleName, "0.0.1");

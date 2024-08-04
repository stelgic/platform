#pragma once

#include <fftwpp/fftw++.h>
#include <IDataProcessor.h>

using namespace stelgic;
using ComplexVector = std::vector<std::complex<double>>;
using Complex2DVector = std::vector<ComplexVector>;

/**
 * initialize fftw_plan and inputs 
 * to be use on next evaluation
 */
struct FFTNOISE {
    FFTNOISE() {}
    virtual ~FFTNOISE() {
        if(out_plan)
            fftw_destroy_plan(out_plan);
        if(in_plan)
            fftw_destroy_plan(in_plan);

        out_plan = nullptr;
        in_plan = nullptr;
    }

    fftw_plan in_plan;
    fftw_plan out_plan;
    ComplexVector inputFreq;
    ComplexVector outFreq;
};

/**
 * Implements FFT LOW pass filter processor module
 */
class FFTNoiseProcessor : public IDataProcessor
{
public:
    using Tag = IndicTag<FFTNOISE>;
    static constexpr const char* moduleName = "fftnoise";

public:
    FFTNoiseProcessor() { fftw_make_planner_thread_safe(); }
    virtual ~FFTNoiseProcessor() {}

    virtual TaskArray Evaluate(
        StrategyInputData& dataRef, size_t aSize, IndicParams &data, 
        const double &errorVal, bool& allocate, std::atomic_bool& success) override;

    std::vector<std::string> const& GetOutNames() const override 
    {
        return outNames;
    }

    DataTag GetDataTag() override { return DataTags::OHLCV(); }

protected:
    std::vector<std::string> outNames = {"FFTNOISE"};
};



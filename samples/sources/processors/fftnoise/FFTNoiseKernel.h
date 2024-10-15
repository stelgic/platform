#pragma once

#define NOMINMAX
#define _USE_MATH_DEFINES

#include <fftwpp/fftw++.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <deque>
#include <vector>
#include <TableColumnHelper.h>

inline bool FFTNoiseFilter(size_t idx, const double* input_values, 
    size_t dataLen, size_t aSize, fftw_plan& in_plan, fftw_plan& out_plan, 
    fftw_complex* inputFreq, fftw_complex* outFreq, double *output_values, 
    int cutOff, int field, int nfields, const double *errorVal)
{
    size_t halfPoints = 1;
    size_t instrumOffset = 0;
    // check if pointers are null
    bool success = (input_values && in_plan && out_plan && 
                    inputFreq && outFreq && output_values);
    if(!success) goto END;

    instrumOffset = idx * dataLen;

    // copy input real
    for(size_t i = 0; i < dataLen; ++i)
    {
        inputFreq[i][0] = input_values[i];
        inputFreq[i][1] = 0.0;
    } 

    //performs the fourier transform to frequency domain
    fftw_execute_dft(in_plan, inputFreq, outFreq);

    // Apply low-pass filter in frequency domain
    halfPoints = dataLen/2;
    for(size_t i = 0; i <= halfPoints; ++i)
    {
        if(i > cutOff)
        {
            outFreq[i][0] = 0.0;
            outFreq[i][1] = 0.0;
        }

        const int64_t j = halfPoints + i;
        if(j >= dataLen) break;

        if(i > cutOff)
        {
            outFreq[j][0] = 0.0;
            outFreq[j][1] = 0.0;
        }
    }

    // performs frequency to time domain inverse transformation
    fftw_execute_dft(out_plan, outFreq, inputFreq);

    // normalize output real
    for(size_t i = 0; i < dataLen; ++i)
    {
        output_values[instrumOffset+i] = inputFreq[i][0] / dataLen;
    }

    END:
    return success;
}


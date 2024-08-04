#pragma once

#include <iostream>

inline bool ExpMovingAverage(
    size_t idx, const double* input_values, size_t dataLen, 
    size_t aSize, double *output_values, int winSize, int field, int nfields, 
    const double *errorVal, double* cachedPrevEmas, bool normalized, double scale=100.0)
{
    bool success = false;
    if(!input_values || !output_values || !cachedPrevEmas)
        return success;

    size_t i=0;
    size_t count = 0;
    size_t oindex = 0;
    size_t chachedInd = 0;
    double average = 0;
    double factor = 2.0 / (winSize + 1);
    size_t instrumOffset = idx * dataLen; /// compute each symbol start offset in 1D array
    double prevEma = cachedPrevEmas[idx]; // re-use previous computed ema

    // jump to last element and compute ema value using previous cached
    // jump to last element and compute ema value using previous cached
    if(prevEma != *errorVal)
    {
        i = dataLen - 1;
        count = winSize;
    }
    
    for( ; i < dataLen; i++)
    {
        success = true;
        oindex = instrumOffset + i;
        output_values[oindex] = prevEma;

        if(count < winSize)
        {
            if(input_values[i] != *errorVal)
            {
                count++;
                average += input_values[i];      

                if (count == winSize && prevEma == *errorVal)
                {
                    average /= count;
                    prevEma = average;
                    output_values[oindex] = average;
                    if(normalized)
                        output_values[oindex] = (input_values[i] - average) / average * scale;
                }
            }
        }
        else if(input_values[i] != *errorVal && prevEma != *errorVal)
        {
            double ema = ((input_values[i] - prevEma) * factor) + prevEma;
            output_values[oindex] = ema;
            if(normalized && ema != 0)
                output_values[oindex] = (input_values[i] - ema) / ema * scale;
            prevEma = ema;
            success = true;
        }
    }
    
    cachedPrevEmas[idx] = output_values[oindex];
    
    return success;
}


// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#pragma once

#include <cmath>
#include <algorithm>
#include <vector>

inline bool AverageTrueRange(
    size_t idx, const std::vector<const double*>& columns, size_t dataLen, 
    size_t aSize, double *output_values, int winSize, int field, int nfields, 
    const double *errorVal, double* cachedPrevAtrs, double* cachedPrevCloses, 
    bool normalized, double scale=100.0)
{
    bool success = false;
    const double* high_values = columns.at(0);
    const double* low_values = columns.at(1);
    const double* close_values = columns.at(2);

    if(columns.size() == 0 || !output_values || !cachedPrevAtrs || 
        !cachedPrevCloses || !high_values || !low_values || !close_values)
    {
        return success;
    }

    int count = 0;
    size_t i=0;
    size_t oindex = 0;
    double average = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    double highlow = 0.0;
    double highclose = 0.0;
    double lowclose = 0.0;
    double trueRange = 0.0;
    
    size_t instrumOffset = idx * dataLen; /// compute each symbol start offset in 1D array
    double prevAtr = cachedPrevAtrs[idx]; // re-use previous computed atr
    double prevClose = cachedPrevCloses[idx]; // re-use previous close

    // jump to last element and compute ema value using previous cached
    if(prevAtr != *errorVal)
    {
        i = dataLen - 1;
        count = winSize;
    }
    
    for(; i < dataLen; i++)
    {
        oindex = instrumOffset + i;
        output_values[oindex] = *errorVal;

        if(high_values[i] != *errorVal &&
            low_values[i] != *errorVal &&
            close_values[i] != *errorVal)
        {
            high = high_values[i];
            low = low_values[i];
            close = close_values[i];
            highlow = high - low;

            if(prevClose != *errorVal)
            {
                highclose = (high-prevClose);
                lowclose = (prevClose-low);
            }

            trueRange = std::max(std::max(highlow, highclose), lowclose);
            if(normalized && prevClose != 0.0)
                trueRange = (trueRange / prevClose) * scale;

            if(count < winSize && cachedPrevAtrs[idx] == *errorVal)
            {
                count++;
                average += trueRange;
                if(count == winSize)
                {
                    double atr = average / winSize;
                    output_values[oindex] = atr;
                    prevAtr = atr;
                    success = true;
                }
            }
            else if(prevAtr != *errorVal)
            {
                double atr = (prevAtr * (winSize -1) + trueRange) / winSize;
                output_values[oindex] = atr;
                prevAtr = atr;
                success = true;
            }

            prevClose = close;
        }
    }

    cachedPrevCloses[idx] = close;
    cachedPrevAtrs[idx] = output_values[oindex];
    
    return success;
}

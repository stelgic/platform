#pragma once

#include <iostream>
#include <cmath>

/** 
 * This kernel compute Relative Strength Index (RSI)
 * For accurracy propose replace double type with double
*/

inline bool RelativeStrengthIndex(
    size_t idx, const double* input_values, size_t dataLen,  size_t aSize, double *output_values, 
    int winSize, int field, int nfields, const double *errorVal, double* cachedUpAvg, 
    double* cachedDownAvg, double* cachedPrevRsis, double* cachedPrevValues, double scale=100.0)
{
    size_t i=0;
    size_t count = 0;
    size_t oindex = 0;
    double upSum = 0.0;
    double downSum = 0.0;
    size_t instrumOffset = idx * dataLen; /// compute each symbol start offset in 1D array

    size_t cache = 0;
    bool success = false;
    if(!input_values || !output_values || !cachedPrevRsis)
        return success;

    double upAvg = cachedUpAvg[idx];
    double downAvg = cachedDownAvg[idx];
    double prevRsi = cachedPrevRsis[idx];
    double prevValue = cachedPrevValues[idx];

    // jump to last element and compute ema value using previous cached
    if(prevValue != *errorVal)
    {
        i = dataLen - 1;
        count = winSize;
    }
    
    for( ; i < dataLen; i++)
    {
        success = true;
        oindex = instrumOffset + i;
        output_values[oindex] = prevRsi;

        // compute first avg up and down and skip win size if already cached
        if(count < winSize)
        {
            // skip error values
            if(prevValue != *errorVal && input_values[i] != *errorVal)
            {
                double changes = input_values[i] - prevValue;
                if(changes < 0.0)
                    downSum += abs(changes);
                else if(changes > 0.0)
                    upSum += changes;

                ++count;
                if (count == winSize && (downAvg == *errorVal || upAvg == *errorVal))
                {
                    downAvg = downSum / winSize;
                    upAvg = upSum / winSize;
                    // calculate rsi
                    if(upAvg == 0.0 && downAvg == 0.0)
                        output_values[oindex] = scale / 2;
                    else
                    {
                        double rs = (downAvg != 0.0)? upAvg / downAvg: 1.0;
                        output_values[oindex] = (downAvg != 0.0)? scale - (scale / (1 + rs)): scale;
                    }
                    ++count;
                }
            }
        }
        else if(count >= winSize && input_values[i] != *errorVal && 
            prevValue != *errorVal && prevRsi != *errorVal)
        {
            success = true;         
            double upChanges = 0.0;
            double downChanges = 0.0;
            double changes = input_values[i] - prevValue;

            if(changes < 0.0)
                downChanges = abs(changes);
            else if(changes > 0.0)
                upChanges = changes;

            upAvg = (upAvg * (winSize - 1) + upChanges) / winSize;
            downAvg = (downAvg * (winSize - 1) + downChanges) / winSize;
            
            if(upChanges != 0.0 || downChanges != 0.0)
            {
                // calculate rsi
                double rs = (downAvg != 0.0)? upAvg / downAvg: 1.0;
                output_values[oindex] = (downAvg != 0.0)? scale - (scale / (1 + rs)): scale;
                if(abs(output_values[oindex] - prevRsi) > 20)
                {
                    double dir = (upChanges == 0.0)? -1.0: 1.0;
                    output_values[oindex] = prevRsi + (dir * scale / prevRsi);
                }  
            }
            else
            {
                // keep rsi close to 50 when market is not moving
                if(prevRsi > 0.0 && abs(50 - prevRsi) > 1.0)
                {
                    double dir = (prevRsi > 50.0)? -1.0: 1.0;
                    output_values[oindex] = prevRsi + (dir * scale / prevRsi);
                }
                else
                    output_values[oindex] = scale / 2;
            }
        }

        prevRsi = output_values[oindex];
        if(input_values[i] != *errorVal)
            prevValue = input_values[i];

        // cache the last computed RSI for each evaluation batch
        cachedUpAvg[idx] = upAvg;
        cachedDownAvg[idx] = downAvg;
        cachedPrevValues[idx] = prevValue;
        cachedPrevRsis[idx] = prevRsi;
    }

    return success;
}



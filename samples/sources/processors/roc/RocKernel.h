#pragma once

#include <iostream>
#include <vector>
#include <deque>


inline bool RateOfChange(
    size_t idx, const double* input_values, size_t dataLen, size_t aSize, 
    double *output_values, int winSize, int field, int nfields, const double *errorVal, 
    std::vector<std::deque<double>>& cachedPrevCloses, double scale=100.0)
{
    size_t i=0;
    size_t oindex = 0;
    int count = 0;
    bool success = false;
    size_t instrumOffset = idx * dataLen; /// compute each symbol start offset in 1D array
    double prevClose = cachedPrevCloses.at(idx).front(); // re-use previous close
    double prevValue = prevClose;
    double prevRoc = *errorVal;
    
    if(!input_values || !output_values)
        return success;

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
        output_values[oindex] = prevRoc;

        if(input_values[i] != *errorVal)
        {
            count++;
            size_t prevInd = (i - winSize); 
            // cache win size prev close values
            if(count < winSize && cachedPrevCloses[idx].front() == *errorVal) 
            {
                cachedPrevCloses[idx].push_back(input_values[i]);
                while(cachedPrevCloses[idx].size() > winSize)
                    cachedPrevCloses[idx].pop_front();
            }
            else if(prevValue != *errorVal) // cache the new prev close
            {
                cachedPrevCloses[idx].push_back(input_values[i]);
                cachedPrevCloses[idx].pop_front();
            }

            // use cached previous close values         
            if(count < winSize && cachedPrevCloses[idx].at(count-1) != *errorVal)
                prevClose = cachedPrevCloses[idx].at(count-1);
            else if(count >= winSize) 
                prevClose = input_values[prevInd];  
            
            if(prevClose != *errorVal && prevClose != 0.0)
            {
                success = true;
                double roc = (input_values[i] - prevClose) / prevClose * scale;
                output_values[oindex] = roc;  
                prevRoc = roc;          
            }
        }
    }
    
    return success;
}


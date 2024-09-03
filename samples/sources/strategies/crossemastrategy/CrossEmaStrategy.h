// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#pragma once

#include <BaseStrategy.h>

namespace stelgic
{
class CrossEmaStrategy: public BaseStrategy
{
public:
    static constexpr const char* moduleName = "crossemastrategy";

public:
    CrossEmaStrategy(const CrossEmaStrategy& other) = default;
    CrossEmaStrategy& operator=(const CrossEmaStrategy& other) = default;
    
public:
    CrossEmaStrategy() {};
    virtual ~CrossEmaStrategy(){ };

    bool ValidateInputs(const StrategyInputData& input) override;
    void Evaluate(const StrategyInputData& data) override;

protected:
    /**
     * @brief Implment signal generation
     * @param data
     * @return std::vector<Json::Value>
     */
    std::vector<Json::Value> PredictSignal(const StrategyInputData& data) override;

    /**
     * @brief override this method to initialize state map. 
     * 
     */
    virtual void InitStateMap() override;

private:
    // input params indices
    int rsiIdx = -1; 
    int atrIdx = -1; 
    int shortEmaIdx = -1;
    int midEmaIdx = -1;
    int longEmaIdx = -1;
};
}

// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#pragma once

#include <BaseStrategy.h>

namespace stelgic
{
class SimpleStrategy: public BaseStrategy
{
public:
    static constexpr const char* moduleName = "simplestrategy";

public:
    SimpleStrategy(const SimpleStrategy& other) = default;
    SimpleStrategy& operator=(const SimpleStrategy& other) = default;
    
public:
    SimpleStrategy() {};
    virtual ~SimpleStrategy(){ };
    
    bool ValidateInputs(const StrategyInputData& input) override;
    void Evaluate(const StrategyInputData& input) override;

protected:
    std::vector<Json::Value> PredictSignal(const StrategyInputData& data) override;
    virtual void InitStateMap() override;
};
}


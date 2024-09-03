// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#include <IMetrics.h>

namespace stelgic
{
class StandardMetrics: public IMetrics
{
public:
    static constexpr const char* moduleName = "standards";

public:
    StandardMetrics(const StandardMetrics& other) = default;
    StandardMetrics& operator=(const StandardMetrics& other) = default;

public:
    StandardMetrics() {}
    virtual ~StandardMetrics() {}

    bool IsInitialized() override;
    std::string GetName() override;
    const MetricsCollection::value_type::second_type& GetKernels() const override;
    const MetricsCollection::value_type::second_type& GetMissingKernels() const override;
    bool Init(const MetricsCollection::value_type::second_type& collection, 
            g3::LogWorker* logWorker, int verbosity) override;
    void Evaluate(MetricReferenceData& data) override;

private:
    void InitializeOutputs(const stelgic::MetricParams &metric);

private:
    MetricsCollection::value_type::second_type metrics;
    MetricsCollection::value_type::second_type missings;
    std::atomic<int> verbose = ATOMIC_FLAG_INIT; 
};
}

#include <IMetrics.h>

namespace stelgic
{
struct MonteCarloStats
{
    double bestValue = std::numeric_limits<double>::min();
    double worseValue = std::numeric_limits<double>::max();
    double averageValue = 0;
};

class MonteCarlo: public IMetrics
{
public:
    static constexpr const char* moduleName = "montecarlo";

public:
    MonteCarlo(const MonteCarlo& other) = default;
    MonteCarlo& operator=(const MonteCarlo& other) = default;

public:
    MonteCarlo() {}
    virtual ~MonteCarlo() {}

    bool IsInitialized() override;
    std::string GetName() override;
    const MetricsCollection::value_type::second_type& GetKernels() const override;
    const MetricsCollection::value_type::second_type& GetMissingKernels() const override;
    bool Init(const MetricsCollection::value_type::second_type& collection,
                g3::LogWorker* logWorker, int verbosity) override;
    void Evaluate(MetricReferenceData& data) override;

private:
    std::default_random_engine rng;
    MetricsCollection::value_type::second_type metrics;
    MetricsCollection::value_type::second_type missings;
    std::atomic<int> verbose = ATOMIC_FLAG_INIT; 
};
}

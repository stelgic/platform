#include <random>
#include "Kernels.hpp"
#include "MonteCarlo.h"

namespace stelgic
{
bool MonteCarlo::Init(
    const MetricsCollection::value_type::second_type& collection, g3::LogWorker* logWorker, int verbosity)
{
#if defined(_WIN32) || defined(_WIN64)
        if(logWorker != nullptr && !g3::internal::isLoggingInitialized())
            g3::initializeLogging(logWorker);
#endif 
    verbose = verbosity;
    // Randomizer
    rng = std::default_random_engine {};

    metrics.clear();
    IMetricKernel::kernelInstances.clear();
    for(auto& metric: collection)
    {
        metric.instances.clear();
        if(metric.active)
        {
            if(IMetricKernel::kernelRegistry.count(metric.name))
            {
                IMetricKernel::KernelFactory kernel = IMetricKernel::kernelRegistry.at(metric.name);
                std::shared_ptr<IMetricKernel> kernelPtr(kernel());
                IMetricKernel::kernelInstances.insert_or_assign(metric.name, kernelPtr);
                metric.instances.push_back({metric.name, IMetricKernel::kernelInstances.at(metric.name)});
                metric.instances.back().kernelPtr->Reset();
                metric.source = "montecarlo"; // source table to query data
                
                // add multiple instance of same kernel for Monte carlo
                int factor = metric.attrs.get("factor", 0).asInt();
                for(int j=0; j < factor; ++j)
                {
                    std::shared_ptr<IMetricKernel> kernelPtr(kernel());
                    std::string colName = metric.name + std::to_string(j);
                    IMetricKernel::kernelInstances.insert_or_assign(colName, kernelPtr);
                    metric.instances.push_back({colName, IMetricKernel::kernelInstances.at(colName)});
                    metric.instances.back().kernelPtr->Reset();
                }

                if(metric.showStats) 
                {
                    for(auto field: std::vector<std::string>{"Best","Worse","Avg."})
                    {
                        metric.stats["values"].append(0.0);
                        metric.stats["labels"].append(field+" "+metric.label);
                        metric.stats["colors"].append(kernelPtr->ColorCode());
                        metric.stats["valueTypes"].append("double");
                    }
                }

                metrics.insert(metric);
            }
            else
                missings.insert(metric);
        }
    }

    return (metrics.size() > 0 && logWorker != nullptr);
}

bool MonteCarlo::IsInitialized()
{
    return (metrics.size() > 0);
}

std::string MonteCarlo::GetName()
{
    return moduleName;
}

const MetricsCollection::value_type::second_type& MonteCarlo::GetKernels() const
{
    return metrics;
}

const MetricsCollection::value_type::second_type& MonteCarlo::GetMissingKernels() const
{
    return missings;
}

void MonteCarlo::Evaluate(MetricReferenceData& data)
{
    // TODO: call all active metrics specified by user
    auto column = data.trades->GetColumnByName(data.colName);
    const double* pnls = castTableColumn<double>(column->chunk(0));
    auto& columnRawPointers = data.columnsRawValues.at(moduleName);
    const int64_t rows = data.trades->num_rows();
    std::uniform_int_distribution<int64_t> dist(0, rows-1);

    const size_t metricsCount = metrics.size();
    std::vector<MonteCarloStats> monteCarloStats(metricsCount);

    for(int64_t i=0; i < data.trades->num_rows() && pnls; ++i)
    {
        size_t m = 0;
        for(auto& metric: metrics)
        {
            size_t j = 0;
            for(auto[name, kernelPtr]: metric.instances)
            {
                if(j > 0)
                {
                    int64_t index = dist(rng); 
                    kernelPtr->Update(pnls, index, i, columnRawPointers.at(name));
                }
                else // default curve
                    kernelPtr->Update(pnls, i, i, columnRawPointers.at(name));
                ++j;
            }

            if(j > 1) ++m;
        }
    }

    // compute metrics stats
    size_t m = 0;
    for(auto& metric: metrics)
    {
        // TODO: skipping compute statistics values for metrics with show off
        if(!metric.showStats) continue;

        size_t j = 0;
        for(auto[name, kernelPtr]: metric.instances)
        {
            if(j > 0)
            {
                double value = kernelPtr->GetValue().asDouble();
                monteCarloStats.at(m).averageValue += value;
                monteCarloStats.at(m).worseValue = std::min(monteCarloStats.at(m).worseValue, value);
                // override best drawdown if zero
                if(metric.name == "maxdrawdown")
                {
                    if(value < -0.1)
                    {
                        if(monteCarloStats.at(m).bestValue >= -0.01) // use best drawdown below -0.01 only
                            monteCarloStats.at(m).bestValue = value;
                        monteCarloStats.at(m).bestValue = std::max(monteCarloStats.at(m).bestValue, value);
                    }
                }
                else
                    monteCarloStats.at(m).bestValue = std::max(monteCarloStats.at(m).bestValue, value);
            }
            ++j;
        }

        int factor = metric.attrs.get("factor", 1).asInt();
        metric.stats["values"][0] = monteCarloStats.at(m).bestValue;
        metric.stats["values"][1] = monteCarloStats.at(m).worseValue;
        metric.stats["values"][2] = monteCarloStats.at(m).averageValue / factor;

        metric.stats["colors"][0] = (monteCarloStats.at(m).bestValue >= 0)? 1: 3;
        metric.stats["colors"][1] = (monteCarloStats.at(m).worseValue >= 0)? 3: 2;
        metric.stats["colors"][2] = (monteCarloStats.at(m).averageValue >= 0)? 0: 3;
        ++m;
    }
}
}

METRICS_MODULE(stelgic::MonteCarlo, stelgic::MonteCarlo::moduleName, "0.0.1");


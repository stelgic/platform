// Copyright Stelgic Fintech Ltd. All Rights Reserved.

#include "Kernels.hpp"
#include "StandardMetrics.h"

namespace stelgic
{
bool StandardMetrics::Init(
    const MetricsCollection::value_type::second_type& collection, g3::LogWorker* logWorker, int verbosity)
{
#if defined(_WIN32) || defined(_WIN64)
        if(logWorker != nullptr && !g3::internal::isLoggingInitialized())
            g3::initializeLogging(logWorker);
#endif 
    verbose = verbosity;

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
                metric.instances.push_back({metric.name, IMetricKernel::kernelInstances.at(metric.name).get()});
                metric.instances.back().kernelPtr->reset();
                metric.source = "trades"; // source table to query data
                
                Json::Value labels(Json::arrayValue);
                labels.append(metric.label);
                metric.stats["labels"] = metric.attrs.get("labels", labels);

                InitializeOutputs(metric);
                metrics.insert(metric);
            }
            else
                missings.insert(metric);
        }
    }

    return (metrics.size() > 0 && logWorker != nullptr);
}

bool StandardMetrics::IsInitialized()
{
    return (metrics.size() > 0);
}

std::string StandardMetrics::GetName()
{
    return moduleName;
}

const MetricsCollection::value_type::second_type& StandardMetrics::GetKernels() const
{
    return metrics;
}

const MetricsCollection::value_type::second_type& StandardMetrics::GetMissingKernels() const
{
    return missings;
}

void StandardMetrics::InitializeOutputs(const stelgic::MetricParams &metric)
{
    auto ParseType = [&](const Json::Value& value)
    {
        switch (value.type())
        {
        case Json::ValueType::intValue:
            if(value.isInt64())
                metric.stats["valueTypes"].append("int64");
            else if(value.isUInt64())
                metric.stats["valueTypes"].append("uint64");
            else if(value.isUInt())
                metric.stats["valueTypes"].append("uint");
            else
                metric.stats["valueTypes"].append("int");
            break;
        case Json::ValueType::realValue:
            metric.stats["valueTypes"].append("double");
            break;
        case Json::ValueType::objectValue:
            metric.stats["valueTypes"].append("json");
            break;
        case Json::ValueType::arrayValue:
            metric.stats["valueTypes"].append("jsonArray");
            break;
        default:
            metric.stats["valueTypes"].append("undefined");
            break;
        }
    };

    for(auto[name, kernelPtr]: metric.instances)
    {
        int colorCode = kernelPtr->ColorCode();
        Json::Value& dataValue = kernelPtr->GetValue();
        if(kernelPtr->GetType() == OutType::JSON)
        {
            for(const auto& field: metric.attrs["fields"])
            {
                const Json::Value& value = dataValue[field.asString()];
                metric.stats["colors"].append(colorCode);
                metric.stats["values"].append(value);

                ParseType(value);
            }
        }
        else
        {
            metric.stats["colors"].append(colorCode);
            metric.stats["values"].append(dataValue);
            ParseType(dataValue);
        }
    }
}

void StandardMetrics::Evaluate(MetricReferenceData& data)
{
    // TODO: call all active metrics specified by user
    auto column = data.trades->GetColumnByName(data.colName);
    const double* pnls = castTableColumn<double>(column->chunk(0));
    auto& columnRawPointers = data.columnsRawValues.at(moduleName);
    for(int64_t i=0; i < data.trades->num_rows() && pnls; ++i)
    {
        for(auto& metric: metrics)
        {
            for(auto[name, kernelPtr]: metric.instances)
                kernelPtr->update(pnls, i, i, columnRawPointers.at(name));
        }
    }

    // update metrics values
    for(auto& metric: metrics)
    {
        // TODO: skipping compute statistics values for metrics with show off
        if(!metric.showStats) continue;

        Json::Value::ArrayIndex j = 0;
        for(auto[name, kernelPtr]: metric.instances)
        {
            int colorCode = kernelPtr->ColorCode();
            Json::Value dataValue = kernelPtr->GetValue();
            if(kernelPtr->GetType() == OutType::JSON)
            {
                for(const auto& field: metric.attrs["fields"])
                {
                    const Json::Value& value = dataValue[field.asString()];
                    metric.stats["colors"][j] = colorCode;
                    metric.stats["values"][j] = value;
                    ++j;
                }
            }
            else
            {
                metric.stats["colors"][j] = colorCode;
                metric.stats["values"][j] = dataValue;
            }
        }
    }
}
}

METRICS_MODULE(stelgic::StandardMetrics, stelgic::StandardMetrics::moduleName, "0.0.1");


#include <gtest/gtest.h>

#include <memory>
#include <ModuleLoader.h>
#include <ModuleUtils.h>
#include "../common/TestUtils.h"

namespace stelgic
{
namespace tests
{
namespace
{
	// The fixture for testing class Foo.
	class DatasetHelperTest : public testing::Test 
	{
	protected:
		DatasetHelperTest() {
			// You can do set-up work for each test here.
		}

		~DatasetHelperTest() override {
			// You can do clean-up work that doesn't throw exceptions here.
		}

		void SetUp() override {
			// Code here will be called immediately after the constructor (right before each test).
		}

		void TearDown() override {
			// Code here will be called immediately after each test (right
			// before the destructor).
			g3::internal::shutDownLogging();
		}
	};

	// get dataset fragments
	TEST_F(DatasetHelperTest, LoadConnectorModule) {
		ModuleInstances moduleInstances;
		MetricsModulesInfo metricsModulesInfo;
		std::atomic<int> verboseLevel = {0};
		std::string moduleName = "binance";

		std::string moduleDir = std::filesystem::canonical("../../modules/connectors").string();
		GTEST_COUT << "Loading " << (moduleDir+ "/" + moduleName + ".dll") << "\n";		

		auto result = ModuleUtils::GetModules(moduleDir, metricsModulesInfo);
		ASSERT_TRUE(result.first) << result.second;
		
		auto [success, err, connectorModule] = ModuleUtils::OpenModule(
			metricsModulesInfo, moduleName, EModule::Connector);
		ASSERT_TRUE(success) << err;
	}

	TEST_F(DatasetHelperTest, LoadMetricModule) {
		ModuleInstances moduleInstances;
		MetricsModulesInfo metricsModulesInfo;
		std::atomic<int> verboseLevel = {0};
		std::string metricName = "standards";

		std::string moduleDir = std::filesystem::canonical("../../modules/metrics").string();	
		GTEST_COUT << "Loading " << (moduleDir+ "/" + metricName + ".dll") << "\n";

		auto result = ModuleUtils::GetModules(moduleDir, metricsModulesInfo);
		ASSERT_TRUE(result.first) << result.second;
		
		auto [success, err, metricModule] = ModuleUtils::OpenModule(
			metricsModulesInfo, metricName, EModule::Metrics);
		ASSERT_TRUE(success) << err;
	}

	TEST_F(DatasetHelperTest, LoadParserModule) {
		ModuleInstances moduleInstances;
		MetricsModulesInfo metricsModulesInfo;
		std::atomic<int> verboseLevel = {0};
		std::string moduleName = "binanceparser";

		std::string moduleDir = std::filesystem::canonical("../../modules/parsers").string();	
		GTEST_COUT << "Loading " << (moduleDir+ "/" + moduleName + ".dll") << "\n";

		auto result = ModuleUtils::GetModules(moduleDir, metricsModulesInfo);
		ASSERT_TRUE(result.first) << result.second;
		
		auto [success, err, metricModule] = ModuleUtils::OpenModule(
			metricsModulesInfo, moduleName, EModule::Parser);
		ASSERT_TRUE(success) << err;
	}

	TEST_F(DatasetHelperTest, LoadEmaProcessorModule) {
		ModuleInstances moduleInstances;
		ProcessorModulesInfo processorModulesInfo;
		std::atomic<int> verboseLevel = {0};
		std::string moduleName = "ema";

		std::string moduleDir = std::filesystem::canonical("../../modules/processors").string();
		GTEST_COUT << "Loading " << (moduleDir+ "/" + moduleName + ".dll") << "\n";

		auto result = ModuleUtils::GetModules(moduleDir, processorModulesInfo);
		ASSERT_TRUE(result.first) << result.second;
		
		auto [success, err, processorModule] = ModuleUtils::OpenModule(
			processorModulesInfo, moduleName, EModule::Processor);
		ASSERT_TRUE(success) << err;
	}

	TEST_F(DatasetHelperTest, LoadRocProcessorModule) {
		ModuleInstances moduleInstances;
		ProcessorModulesInfo processorModulesInfo;
		std::atomic<int> verboseLevel = {0};
		std::string moduleName = "roc";

		std::string moduleDir = std::filesystem::canonical("../../modules/processors").string();
		GTEST_COUT << "Loading " << (moduleDir+ "/" + moduleName + ".dll") << "\n";

		auto result = ModuleUtils::GetModules(moduleDir, processorModulesInfo);
		ASSERT_TRUE(result.first) << result.second;
		
		auto [success, err, processorModule] = ModuleUtils::OpenModule(
			processorModulesInfo, moduleName, EModule::Processor);
		ASSERT_TRUE(success) << err;
	}

	TEST_F(DatasetHelperTest, LoadRsiProcessorModule) {
		ModuleInstances moduleInstances;
		ProcessorModulesInfo processorModulesInfo;
		std::atomic<int> verboseLevel = {0};
		std::string moduleName = "rsi";

		std::string moduleDir = std::filesystem::canonical("../../modules/processors").string();
		GTEST_COUT << "Loading " << (moduleDir+ "/" + moduleName + ".dll") << "\n";

		auto result = ModuleUtils::GetModules(moduleDir, processorModulesInfo);
		ASSERT_TRUE(result.first) << result.second;
		
		auto [success, err, processorModule] = ModuleUtils::OpenModule(
			processorModulesInfo, moduleName, EModule::Processor);
		ASSERT_TRUE(success) << err;
	}

	// get dataset fragments
	TEST_F(DatasetHelperTest, ConnectorModuleLoader) {
		std::string moduleName = "polygon.dll";
		auto filePath = std::filesystem::canonical("../../modules/connectors");	
		filePath.append(moduleName);

		GTEST_COUT << "Loading " << filePath.string() << "\n";

		auto modPtr = std::make_unique<ModuleLoader<IConnector>>(filePath.string());
		bool ok = false;
		std::string error;
		std::tie(ok, error) = modPtr->Open();
		if (ok)
			modPtr->Close(); 

		ASSERT_TRUE(ok) << "ERROR LOADING " << filePath.string() << " [" << error << "]\n";
	}

	// get dataset fragments
	TEST_F(DatasetHelperTest, MetricModuleLoader) {
		std::string moduleName = "standards.dll";
		auto filePath = std::filesystem::canonical("../../modules/metrics");	
		filePath.append(moduleName);

		GTEST_COUT << "Loading " << filePath.string() << "\n";

		auto modPtr = std::make_unique<ModuleLoader<IMetrics>>(filePath.string());
		bool ok = false;
		std::string error;
		std::tie(ok, error) = modPtr->Open();
		if (ok)
			modPtr->Close(); 

		ASSERT_TRUE(ok) << "ERROR LOADING " << filePath.string() << " [" << error << "]\n";
	}

#if defined(_WIN32) || defined(_WIN64)
	TEST_F(DatasetHelperTest, LoadConnectorModuleDirect) {
		bool loaded = false;
		char* (*name)();
		char* (*version)();
		IConnector* (*create)();

		std::string moduleName = "binance.dll";
		auto filePath = std::filesystem::canonical("../../modules/connectors");	
		filePath.append(moduleName);

		GTEST_COUT << "Loading " << filePath.string() << "\n";

		HMODULE handle = LoadLibrary(filePath.string().c_str());
        if (handle != NULL)
        {
            create = (IConnector* (*)())GetProcAddress(handle, "Create");
            name = (char* (*)())GetProcAddress(handle, "Name");
            version = (char* (*)())GetProcAddress(handle, "Version");
            if (create != NULL && name != NULL && version != NULL)
                loaded = true;
        }
        
		if(handle != NULL)
			FreeLibrary(handle);
		ASSERT_TRUE(loaded) << std::system_category().message(GetLastError());
	}

	TEST_F(DatasetHelperTest, LoadWsAddon) {
		bool loaded = false;
		char* (*name)();
		char* (*version)();
		IConnector* (*create)();

		std::string moduleName = "wsqcraftor.node";
		auto filePath = std::filesystem::canonical("../../ws-addons/build/Release");	
		filePath.append(moduleName);

		GTEST_COUT << "Loading " << filePath.string() << "\n";

		HMODULE handle = LoadLibrary(filePath.string().c_str());
		if(handle != NULL)
			FreeLibrary(handle);
		ASSERT_TRUE(loaded) << std::system_category().message(GetLastError());
	}
#endif
}
}
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


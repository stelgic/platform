#include <fstream>
#include <filesystem>
#include <json/json.h>

#include "../common/CommonMacros.h"

namespace stelgic
{
namespace tests
{
namespace
{
class TestUtils
{
public:
    static std::tuple<Json::Value, std::string> LoadJsonFromFile(const std::string &filename)
    {
        std::string errs;
        Json::Value data;
        Json::CharReaderBuilder rbuilder;

        std::ifstream stream;
        stream.open(filename);

        if (stream.is_open())
        {
            if (!Json::parseFromStream(rbuilder, stream, &data, &errs))
                data = Json::Value();
            
            stream.close();
        }

        return std::forward_as_tuple(data, errs);
    }

    static arrow::Result<std::shared_ptr<arrow::dataset::Dataset>> LoadDataset(
        const std::string& dirname, arrow::FieldVector fields,
        std::shared_ptr<arrow::dataset::CsvFileFormat> format,
        std::shared_ptr<arrow::Schema> partSchema)
    {
        std::string rootDir;
		format = std::make_shared<arrow::dataset::CsvFileFormat>();

        auto fsOp = arrow::fs::FileSystemFromUriOrPath(dirname, &rootDir);
		auto filesystem = fsOp.ValueUnsafe();

		return DatasetHelper::GetDataset(filesystem, format, rootDir, partSchema,
                    std::make_shared<arrow::dataset::FilenamePartitioning>(partSchema));
    }
};

}
}
}


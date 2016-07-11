#pragma once
#include <string>

namespace cgvkp {
namespace util {

    class resource_file {
    public:
		static inline std::string getResourcePath(std::string const& pDirectoryName, std::string const& pFilename) { return resources_path + '/' + pDirectoryName + '/' + pFilename; }
        static std::string find_resource_file(std::string const& name);
        static bool read_file_as_text(std::string const& filename, std::string& string);
        static std::string resources_path;

    private:
        resource_file() = delete;
    };

}
}

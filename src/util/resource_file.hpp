#pragma once
#include <string>

namespace cgvkp {
namespace util {

    class resource_file {
    public:

        static std::string find_resource_file(const char *name);
        static std::string read_file_as_text(const std::string& path);

    private:
        resource_file() = delete;
    };

}
}

#pragma once
#include <string>

namespace cgvkp {
namespace util {

    class resource_file {
    public:

        static std::string find_resource_file(const char *name);
        static bool read_file_as_text(char const* filename, std::string& string);

    private:
        resource_file() = delete;
    };

}
}

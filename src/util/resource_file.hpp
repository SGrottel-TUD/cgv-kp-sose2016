#pragma once
#include <string>

namespace cgvkp {
namespace util {

    class resource_file {
    public:

        static std::string find_resource_file(std::string const& name);
        static bool read_file_as_text(std::string const& filename, std::string& string);

    private:
        resource_file() = delete;
    };

}
}

#include "util/resource_file.hpp"
#include <fstream>
#include <vector>

std::string cgvkp::util::resource_file::find_resource_file(const char *name) {
    std::string rel_dir;
    std::string path;

    for (int it = 0; it < 4; it++) {

        path = rel_dir + name;

        std::ifstream test(path.c_str());
        if (test.is_open()) {
            test.close();
            return path;
        }

        path = rel_dir + "res/" + name;

        test.open(path.c_str());
        if (test.is_open()) {
            test.close();
            return path;
        }

        rel_dir += "../";
    }

    // resource not found
    return "";
}

std::string cgvkp::util::resource_file::read_file_as_text(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return nullptr;
    file.seekg(0, std::ifstream::end);
    unsigned int len = static_cast<unsigned int>(file.tellg());
    std::vector<char> cont;
    cont.resize(len);
    file.seekg(0, std::ifstream::beg);
    file.read(cont.data(), len);
    return std::string(cont.data(), len);
}

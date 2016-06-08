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

bool cgvkp::util::resource_file::read_file_as_text(std::string const& filename, std::string& string)
{
    std::ifstream file(filename, std::ios::binary);
	if (file)
	{
		file.seekg(0, std::ios::end);
		string.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&string[0], string.size());

		return true;
	}
	else
	{
		return false;
	}
}

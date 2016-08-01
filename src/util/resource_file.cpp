#include "util/resource_file.hpp"
#include <fstream>

// Default value
std::string cgvkp::util::resource_file::resources_path = "src/resources";

bool cgvkp::util::resource_file::readTextfile(char const* pDirectoryName, char const* pFilename, std::string& string)
{
	std::ifstream file(getResourcePath(pDirectoryName, pFilename), std::ios::binary);
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

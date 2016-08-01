#pragma once
#include <string>

namespace cgvkp
{
	namespace util
	{
		class resource_file
		{
		public:
			static inline std::string getResourcePath(char const* pDirectoryName, std::string const& filename) { return resources_path + '/' + pDirectoryName + '/' + filename; }
			static bool readTextfile(char const* pDirectoryName, char const* pFilename, std::string& string);
			static std::string resources_path;

		private:
			resource_file() = delete;
		};
	}
}

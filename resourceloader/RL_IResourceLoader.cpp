#include "RL_IResourceLoader.h"

namespace RL
{
	std::string IResourceLoader::pathToFileName(const std::string &filepath)
	{
		size_t index = filepath.find_last_of('\\') + 1;

		// Name with extension
		std::string name = filepath.substr(index, filepath.length() - index);

		index = name.find_first_of('.');

		return name.substr(0, index);
	}

	std::string IResourceLoader::pathToFolder(const std::string &filepath)
	{
		size_t index2 = filepath.find_last_of('\\');
		size_t index1 = filepath.substr(0, index2).find_last_of('\\');

		return filepath.substr(index1+1, index2 - 1 - index1);
	}
}
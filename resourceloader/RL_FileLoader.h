#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "RL_IResourceLoader.h"

namespace RL
{
	class FileLoader
	{
	private:
		std::unordered_map<std::string, std::vector<std::string>> m_files;

	public:

		// Initialize this file loader to an ERB folder containing a ResourceList.erl
		FileLoader(const std::string &resourceListFolder);
		~FileLoader();

		// Load resources of specified type into a list of definitions
		std::unordered_map<std::string, std::vector<std::pair<std::string, std::int64_t>>> loadResources(RL::IResourceLoader *loader) const;
	};
}
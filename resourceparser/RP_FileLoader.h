#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "RP_IResourceParser.h"

#include "RP_utils.h"

namespace RL
{
	// Forward declared
	class IResourceParser;

	class FileLoader
	{
	protected:
		std::unordered_map<std::string, std::vector<std::string>> m_files;

		std::string m_resourceListFolder;

	public:

		// Initialize this file loader to an ERB folder containing a ResourceList.erl
		FileLoader(const std::string &resourceListFolder);
		~FileLoader();

		// Opens a requested file for reading
		virtual std::istream *openFile(const std::string &filename) const;

		// Load resources of specified type into a list of definitions
		std::unordered_map<std::string, std::vector<std::pair<std::string, std::int64_t>>> loadResources(IResourceParser *loader);
	};
}
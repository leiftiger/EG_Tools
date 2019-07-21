#pragma once

#include "RL_BaseResource.h"

#include <string>
#include <vector>

namespace RL
{
	class IResourceLoader
	{
	public:

		// File extension for this loader
		virtual const char * const extension() const = 0;

		virtual std::vector<BaseResource*> load(const std::string &filename) = 0;

		// Gets the name of the file excluding the extensions from the given file path
		static std::string pathToFileName(const std::string &filepath);

		// Gets the folder this file is in according to the given file path
		static std::string pathToFolder(const std::string &filepath);
	};
}
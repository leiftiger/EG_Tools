#pragma once
#pragma GCC diagnostic ignored "-Wignored-qualifiers" // We know that virtual overrides don't need virtual, but it's nicer that way!

#include "RP_BaseResource.h"
#include "RP_FileLoader.h"

#include <string>
#include <vector>

namespace RL
{
	// Forward declared
	class FileLoader;

	class IResourceParser
	{
	public:

		virtual ~IResourceParser() = default;

		// File extension for this loader
		virtual const char * const extension() const = 0;

		virtual std::vector<BaseResource*> parse(const std::string &filename, const FileLoader &loader) = 0;

		// Gets the name of the file excluding the extensions from the given file path
		static std::string pathToFileName(const std::string &filepath);

		// Gets the folder this file is in according to the given file path
		static std::string pathToFolder(const std::string &filepath);
	};
}
#pragma once

#pragma GCC diagnostic ignored "-Wignored-qualifiers" // We know that virtual overrides don't need virtual, but it's nicer that way!

#include <iostream>
#include <string>
#include <vector>

#include "ModPack.h"

// Forward declared
class ModPack;

// A patch applied to current resources
class IResourcePatch
{
protected:

	// Files that the patch applies to
	std::vector<std::string> m_filenames;

	// Original mod files that may be copied directly if a mod introduced a new file
	std::vector<std::string> m_modFiles;

	// Buffer size to use when copying files
	const int COPY_BUFFER_SIZE = 4096;

public:

	IResourcePatch(const std::initializer_list<std::string> &filenames, const std::initializer_list<std::string> &modFilenames);

	virtual ~IResourcePatch() = default;

	// Files that the patch applies to
	const std::vector<std::string> &filenames() const;

	// Applies the patch on the input stream through the output stream, returning a list of strings mentioning any errors that occured during the patch
	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const = 0;

	// Applies the patch as a new file as there was no existing file to use
	virtual std::vector<std::string> apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const;
};


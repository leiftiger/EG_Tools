#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "ModPack.h"

// A patch applied to current resources
class IResourcePatch
{
protected:

	// File that the patch applies to
	std::string m_filename;

	// Original mod file that may be copied directly if a mod introduced a new file
	std::string m_modFile;

	// Buffer size to use when copying files
	const int COPY_BUFFER_SIZE = 4096;

public:

	IResourcePatch(const std::string &filename, const std::string &modFilename);

	// File that the patch applies to
	const std::string &filename() const;

	// Applies the patch on the input stream through the output stream, returning a list of strings mentioning any errors that occured during the patch
	virtual std::vector<std::string> apply(std::istream &in, std::ostream &out) const = 0;

	// Applies the patch as a new file as there was no existing file to use
	virtual std::vector<std::string> apply(const ModPack &mod, std::ostream &out) const;
};


#pragma once

#include "ResourcePack.h"

#include <unordered_map>
#include <vector>

class ResourcePacks
{
protected:

	std::vector<ResourcePack*> m_resourcePacks;

	std::unordered_map<std::string, ResourcePack*> m_fileToPack;

	std::vector<std::string> m_files;

	std::unordered_map<std::string, std::string> m_expandedFilenames;

public:

	ResourcePacks(const std::vector<std::string> &filenames);
	~ResourcePacks();

	// Returns true if any resource pack contains the given filename
	bool contains(const std::string &filename) const;

	// Opens a file for reading from any resource packs contained here
	std::istream *openFile(const std::string &filename) const;

	// Retrieves a list of filenames stored in any resource packs
	const std::vector<std::string> &files() const;
};


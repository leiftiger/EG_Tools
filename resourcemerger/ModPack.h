#pragma once

#include <fstream>
#include <vector>

#include <unordered_map>

// A collection of files and references used by a mod
class ModPack
{
protected:

	// Name of mod
	std::string m_name;

	std::vector<std::string> m_files;

	std::unordered_map<int, int> m_descTranslations;

public:

	ModPack(const std::string &name);

	const std::string &name() const;

	// Add a file to this mod pack
	void addFile(const std::string &file);

	// Retrieves a list of files contained within this mod pack
	const std::vector<std::string> &files() const;

	// Opens a file for reading
	std::istream *openFile(const std::string &filename) const;

	// Translates an entity description ID that local mod references used to the globally assigned one
	int translateDescID(int descID);

	void setDescTranslation(int original, int global);
};


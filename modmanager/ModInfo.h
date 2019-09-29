#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

class ModInfo
{
protected:

	std::unordered_map<std::string, std::string> m_properties;

	std::string m_filename;

public:

	std::string &operator[](const std::string &key);

	// Sets the filename
	void setFilename(const std::string &filename);

	const std::string &filename() const;

	static std::string retrieveTagContent(const std::string &tag, const std::string &strSearch);

	friend std::istream &operator>>(std::istream &stream, ModInfo &mod);
	friend std::ostream &operator<<(std::ostream &stream, const ModInfo &mod);
};


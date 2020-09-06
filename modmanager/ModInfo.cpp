#include "ModInfo.h"

#include "utils.h"

std::string &ModInfo::operator[](const std::string &key)
{
	if (m_properties.find(key) == m_properties.end())
		m_properties[key] = "";

	return m_properties[key];
}

void ModInfo::setFilename(const std::string &filename)
{
	m_filename = filename;
}

const std::string &ModInfo::filename() const
{
	return m_filename;
}

std::string ModInfo::retrieveTagContent(const std::string &tag, const std::string &strSearch)
{
	std::regex pattern("(.*)</" + tag + ">");

	std::smatch matches;

	if (std::regex_search(strSearch, matches, pattern))
	{
		return matches[1];
	}

	return "";
}

std::istream &operator>>(std::istream &stream, ModInfo &mod)
{
	std::string line;
	std::string strFile;

	while (util::getline(stream, line))
	{
		strFile += line;
	}

	std::regex pattern("<mod>(.*)</mod>");

	std::smatch matches;

	if (std::regex_search(strFile, matches, pattern))
	{
		strFile = matches[1];

		pattern = std::regex("<([a-z]+)>(.*)");

		while (std::regex_search(strFile, matches, pattern))
		{
			const std::string &tag = matches[1];

			std::string tagContent = mod.retrieveTagContent(tag, matches[2]);

			// Read past content and the open & end tag
			strFile = strFile.substr((tag.length() + 2) * 2 + tagContent.length() + 1);

			mod.m_properties[tag] = tagContent;
		}
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, const ModInfo &mod)
{
	stream << "<mod>" << std::endl;

	for (const std::pair<std::string, std::string> &pair : mod.m_properties)
	{
		std::string key = pair.first;
		std::string value = pair.second;

		std::regex pattern("\n");

		stream << "\t<" << key << ">" << value << "</" << key << ">" << std::endl;
	}

	stream << "</mod>" << std::endl;

	return stream;
}

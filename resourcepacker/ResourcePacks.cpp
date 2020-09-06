#include "ResourcePacks.h"

ResourcePacks::ResourcePacks(const std::vector<std::string> &filenames)
{
	for (const std::string &filename : filenames)
	{
		ResourcePack *pack = nullptr;

		// Only load packs we can load
		try
		{
			pack = new ResourcePack(filename);
		}
		catch (std::exception &e)
		{
			if (pack != nullptr)
				delete pack;

			pack = nullptr;
		}

		if (pack == nullptr)
			continue;

		m_resourcePacks.push_back(pack);

		for (const std::string &packFile : pack->files())
		{
			m_files.push_back(packFile);

			m_fileToPack[packFile] = pack;


			std::string shortName = packFile.substr(packFile.find_last_of('/') + 1);

			m_expandedFilenames[shortName] = packFile;
		}
	}
}

ResourcePacks::~ResourcePacks()
{
	for (ResourcePack *pack : m_resourcePacks)
	{
		delete pack;
	}
}

bool ResourcePacks::contains(const std::string &filename) const
{
	if (m_fileToPack.find(filename) == m_fileToPack.end())
	{
		// Might be the shortened file name
		if (m_expandedFilenames.find(filename) != m_expandedFilenames.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

std::istream *ResourcePacks::openFile(const std::string &filename) const
{
	if (m_fileToPack.find(filename) == m_fileToPack.end())
	{
		// Might be the shortened file name
		if (m_expandedFilenames.find(filename) != m_expandedFilenames.end())
		{
			const std::string &fullName = m_expandedFilenames.at(filename);

			return m_fileToPack.at(fullName)->openFile(fullName);
		}
		else
		{
			throw std::runtime_error(("Couldn't find a pack for file \"" + filename + "\"").c_str());
		}
	}

	return m_fileToPack.at(filename)->openFile(filename);
}

const std::vector<std::string>& ResourcePacks::files() const
{
	return m_files;
}

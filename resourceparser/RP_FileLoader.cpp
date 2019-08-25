#include "RP_FileLoader.h"

namespace RL
{
	FileLoader::FileLoader(const std::string &resourceListFolder) : m_resourceListFolder(resourceListFolder)
	{

	}

	FileLoader::~FileLoader()
	{

	}

	std::istream *FileLoader::openFile(const std::string &filename) const
	{
		return new std::ifstream(filename);
	}

	std::unordered_map<std::string, std::vector<std::pair<std::string, std::int64_t>>> FileLoader::loadResources(IResourceParser *loader)
	{
		// Initialize file list
		// It is called here since we need rely on the openFile virtual function for both resource packs and dynamic resources
		if (m_files.size() == 0)
		{
			std::istream *fs = openFile(m_resourceListFolder + "/ResourceList.erl");

			if (fs->fail())
			{
				delete fs;
				throw std::exception(("Couldn't find folder " + m_resourceListFolder).c_str());
			}

			std::string line;

			// The first 20 lines are seemingly arbitrary numbers
			for (int i = 0; i < 20; i++)
				std::getline(*fs, line);

			while (util::getline(*fs, line))
			{
				size_t index = line.find_last_of('.');

				std::string strExtension = line.substr(index);

				std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);

				if (m_files.find(strExtension) == m_files.end())
					m_files[strExtension] = std::vector<std::string>();

				util::getline(*fs, line);

				m_files[strExtension].push_back(m_resourceListFolder + line.substr(1));

				// Type file
				std::getline(*fs, line);
				// Type name
				std::getline(*fs, line);

				std::getline(*fs, line);

				int numArguments = std::stoi(line);

				// Skip each argument
				for (int i = 0; i < numArguments; i++)
				{
					std::getline(*fs, line);
					std::getline(*fs, line);
				}

				// Read end number
				std::getline(*fs, line);
			}

			delete fs;
		}

		std::unordered_map<std::string, std::vector<std::pair<std::string, std::int64_t>>> res;

		// No files with specified extensions exist
		if (m_files.find(loader->extension()) == m_files.end())
		{
			delete loader;

			return res;
		}

		for (const std::string &file : m_files.at(loader->extension()))
		{
			for (RL::BaseResource *resource : loader->parse(file, *this))
			{
				if (res.find(resource->type()) == res.end())
				{
					res[resource->type()] = std::vector<std::pair<std::string, std::int64_t>>();
				}

				res[resource->type()].push_back(std::make_pair(resource->key(), resource->value()));

				delete resource;
			}
		}

		delete loader;

		return res;
	}
}
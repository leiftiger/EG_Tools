#include "RL_FileLoader.h"

namespace RL
{
	FileLoader::FileLoader(const std::string &resourceListFolder)
	{
		std::ifstream file(resourceListFolder + "\\ResourceList.erl");

		if (file.fail())
			throw std::exception((std::string("Couldn't find folder ") + resourceListFolder).c_str());

		std::string line;

		// The first 20 lines are seemingly arbitrary numbers
		for (int i = 0; i < 20; i++)
			std::getline(file, line);

		while (std::getline(file, line))
		{
			size_t index = line.find_last_of('.');

			std::string strExtension = line.substr(index);

			std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);

			if (m_files.find(strExtension) == m_files.end())
				m_files[strExtension] = std::vector<std::string>();

			std::getline(file, line);

			std::replace(line.begin(), line.end(), '/', '\\');

			m_files[strExtension].push_back(resourceListFolder + line.substr(1));

			// Type file
			std::getline(file, line);
			// Type name
			std::getline(file, line);

			std::getline(file, line);

			int numArguments = std::stoi(line);

			// Skip each argument
			for (int i = 0; i < numArguments; i++)
			{
				std::getline(file, line);
				std::getline(file, line);
			}

			// Read end number
			std::getline(file, line);
		}
	}

	FileLoader::~FileLoader()
	{

	}

	std::unordered_map<std::string, std::vector<std::pair<std::string, std::int64_t>>> FileLoader::loadResources(RL::IResourceLoader *loader) const
	{
		std::unordered_map<std::string, std::vector<std::pair<std::string, std::int64_t>>> res;

		if (m_files.find(loader->extension()) == m_files.end())
			throw std::exception("No files with specified extension exist!");

		for (const std::string &file : m_files.at(loader->extension()))
		{
			for (RL::BaseResource *resource : loader->load(file))
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
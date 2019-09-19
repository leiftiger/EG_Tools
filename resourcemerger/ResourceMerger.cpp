#include "ResourceMerger.h"

#include "IResourcePatch.h"

#include "PatchCopy.h"

#include <algorithm>

ResourceMerger::ResourceMerger(const ResourcePacks *packs, const std::string &outputFolder) : m_resourcePacks(packs), m_outputPath(outputFolder)
{
	std::unordered_map<std::string, int> groups;

	// Scans desc IDs from resource packs to determine various classes and know vacant IDs
	for (const std::string &filePath : packs->files())
	{
		if (filePath.substr(filePath.length() - 5, 5) == ".desc")
		{
			int folderSlash = (int)filePath.find_first_of('/', strlen("./EntityDescriptions/"));

			int fileSlash = (int)filePath.find_first_of('/', folderSlash + 1);

			std::string filename = filePath.substr(fileSlash + 1);

			std::string subFolder = filePath.substr(folderSlash+1, fileSlash - folderSlash - 1);

			std::string masterFolder = filePath.substr(strlen("./EntityDescriptions/"), folderSlash - strlen("./EntityDescriptions/"));

			// Doesn't have a master folder
			if (fileSlash == -1)
			{
				filename = subFolder;
				subFolder = masterFolder;
			}


			int id = std::stoi(filename.substr(0, 5));

			m_reservedDescIDs.emplace(id);

			int minID = std::stoi(subFolder.substr(0, 5));
			int maxID = std::stoi(subFolder.substr(6, 5));

			std::string descClass = subFolder.substr(12);

			groups[descClass] = maxID;

			m_vacantDescIDs[descClass] = minID;

			if (fileSlash != -1)
			{
				minID = std::stoi(masterFolder.substr(0, 5));
				maxID = std::stoi(masterFolder.substr(6, 5));

				descClass = masterFolder.substr(12);

				groups[descClass] = maxID;

				m_vacantDescIDs[descClass] = minID;
			}
		}
	}

	for (const std::pair<std::string, int> &pair : groups)
	{
		m_descClassRanges.push_back(std::make_pair(pair.second, pair.first));
	}

	// A large ID-range isn't used in the base game, but can maybe be used
	m_vacantDescIDs["Unknown"] = 30000;
	m_descClassRanges.push_back(std::make_pair(49999, "Unknown"));

	std::sort(m_descClassRanges.begin(), m_descClassRanges.end(), 
	
		[](const std::pair<int, std::string> &lhs, const std::pair<int, std::string> &rhs)
		{
			return lhs.first < rhs.first;
		}
	);

	std::cout << "hi";
}

ResourceMerger::~ResourceMerger()
{
	delete m_resourcePacks;

	for (ModPack *mod : m_mods)
	{
		delete mod;
	}

	for (IResourcePatcher *patch : m_patchers)
	{
		delete patch;
	}
}

void ResourceMerger::addMod(ModPack *modPack)
{
	m_mods.push_back(modPack);
}

void ResourceMerger::addPatcher(IResourcePatcher *patcher)
{
	m_patchers.push_back(patcher);

	m_extToPatch[patcher->extension()] = patcher;
}

void ResourceMerger::mergeMods(std::ostream &output)
{
	if (m_mods.size() == 1)
	{
		output << "Only one mod is loaded" << std::endl << "Copying files directly from " << m_mods[0]->name() << std::endl;

		const ModPack *mod = m_mods[0];

		char *buffer = new char[COPY_BUFFER_SIZE];

		std::streamsize bufferLength = 0;

		for (const std::string &filename : mod->files())
		{
			std::istream *modFile = mod->openFile(filename);

			std::string mergedName = m_outputPath + "/" + filename.substr(filename.find_last_of('/'));

			std::ofstream mergedFile(mergedName);

			if (mergedFile.fail())
			{
				throw std::exception(("Failed to open \"" + mergedName + "\" for writing (managed)").c_str());
			}

			while (!modFile->eof())
			{
				modFile->read(buffer, COPY_BUFFER_SIZE);

				bufferLength = modFile->gcount();

				mergedFile.write(buffer, bufferLength);
			}

			delete modFile;
		}

		delete[] buffer;
	}
	else
	{
		for (ModPack *mod : m_mods)
		{
			output << "Installing \"" << mod->name() << "\":" << std::endl << std::endl;

			std::vector<IResourcePatch*> patches;

			// Files are grouped by extension as the patcher may be dependent on several files (e.g. ATN global pointers)
			std::unordered_map<std::string, std::vector<std::string>> m_extFiles;

			for (const std::string &fullFilePath : mod->files())
			{
				std::string fileExt = fullFilePath.substr(fullFilePath.find_last_of('.'));

				std::string filename = fullFilePath.substr(fullFilePath.find_last_of('/') + 1);

				// This file isn't permitted to be changed further
				if (m_lockedFiles.find(filename) != m_lockedFiles.end())
				{
					output << "Not including \"" << filename << "\" due to conflicts" << std::endl;
					continue;
				}

				// No patcher found for this filetype
				if (m_extToPatch.find(fileExt) == m_extToPatch.end())
				{
					patches.push_back(new PatchCopy(filename, fullFilePath, *mod));

					lockFile(filename);
				}
				else
				{
					if (m_extFiles.find(fileExt) == m_extFiles.end())
						m_extFiles[fileExt] = std::vector<std::string>();

					m_extFiles[fileExt].push_back(fullFilePath);
				}
			}

			for (const std::pair<std::string, std::vector<std::string>> &pair : m_extFiles)
			{
				std::vector<IResourcePatch*> subPatches = m_extToPatch[pair.first]->createPatches(*this, *mod, pair.second);

				patches.insert(patches.end(), subPatches.begin(), subPatches.end());
			}

			output << "Created " << patches.size() << " patches" << std::endl;

			for (IResourcePatch *patch : patches)
			{
				const std::string &filename = patch->filename();

				std::ifstream fsManaged(m_outputPath + "/" + filename);

				// First occurence of this file
				if (fsManaged.fail())
				{
					fsManaged.close();

					std::ofstream fsOut(m_outputPath + "/" + filename, std::ios::trunc);

					if (fsOut.fail())
					{
						output << "ERROR: Failed to open \"" + filename + "\" for writing (managed)" << std::endl;
					}

					try
					{
						// Patch from base files
						std::istream *fsBase = m_resourcePacks->openFile(filename);

						std::vector<std::string> errors = patch->apply(*fsBase, fsOut);

						for (std::string &err : errors)
						{
							output << err << std::endl;
						}

						delete fsBase;
					}
					catch (std::exception e)
					{
						// This is a file that doesn't exist in the base game, apply directly

						std::vector<std::string> errors = patch->apply(*mod, fsOut);

						for (std::string &err : errors)
						{
							output << err << std::endl;
						}
					}
				}
				else
				{
					// Otherwise, we patch to an already patched file

					// fsIn is a copy of the original since we want to write back to the same file immediately
					std::stringstream fsIn;

					char *buffer = new char[COPY_BUFFER_SIZE];

					std::streamsize bufferLength = 0;

					while (!fsManaged.eof())
					{
						fsManaged.read(buffer, COPY_BUFFER_SIZE);

						bufferLength = fsManaged.gcount();

						fsIn.write(buffer, bufferLength);
					}

					fsManaged.close();

					std::ofstream fsOut(m_outputPath + "/" + filename, std::ios::trunc);

					if (fsOut.fail())
					{
						output << "ERROR: Failed to open \"" + filename + "\" for writing (managed)" << std::endl;
					}

					std::vector<std::string> errors = patch->apply(fsIn, fsOut);

					for (std::string &err : errors)
					{
						output << err << std::endl;
					}

					delete[] buffer;
				}

				delete patch;
			}

			output << "Patches applied" << std::endl << std::endl;
		}
	}
}

void ResourceMerger::lockFile(const std::string &file)
{
	m_lockedFiles.emplace(file);
}

const std::string &ResourceMerger::descClass(int descID)
{
	for (const std::pair<int, std::string> &pair : m_descClassRanges)
	{
		if (descID <= pair.first)
		{
			return pair.second;
		}
	}

	return m_descClassRanges[0].second;
}

int ResourceMerger::reserveDescID(const std::string &descClass)
{
	int cur = m_vacantDescIDs[descClass];

	// Scan for vacant positions (somehow there may be some open slots between base definitions)
	while (m_reservedDescIDs.find(cur) != m_reservedDescIDs.end())
	{
		cur++;
	}

	if (this->descClass(cur) != descClass)
		throw std::exception(("Ran out of entity description IDs for class \"" + descClass + "\"").c_str());

	m_vacantDescIDs[descClass] = cur + 1;

	return cur;
}

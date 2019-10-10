#include "ResourceMerger.h"

#include "IResourcePatch.h"

#include "PatchCopy.h"

#include "StringList.h"

#include <algorithm>
#include <thread>

void ResourceMerger::initialize(Mutex::Server &mutex, double maxPerc)
{
	if (m_initialized)
		return;

	mutex.queue.push("Processing base files for reserved IDs...\n\n");

	mutex.progress = 0;

	mutex.wait();

	m_initialized = true;

	int numFilesProcessed = 0;

	double numFiles = (double)m_resourcePacks->files().size();

	std::unordered_map<std::string, int> groups;

	// Scans desc IDs from resource packs to determine various classes and know vacant IDs
	for (const std::string &filePath : m_resourcePacks->files())
	{
		if (filePath.substr(filePath.length() - 5, 5) == ".desc")
		{
			int folderSlash = (int)filePath.find_first_of('/', strlen("./EntityDescriptions/"));

			int fileSlash = (int)filePath.find_first_of('/', folderSlash + 1);

			std::string filename = filePath.substr(fileSlash + 1);

			std::string subFolder = filePath.substr(folderSlash + 1, fileSlash - folderSlash - 1);

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
		else if (filePath.substr(filePath.length() - 4, 4) == ".ros")
		{
			// As we only care about the IDs, we only need to run the first ATN parse pass
			ATN::List<ATN::Entry> list;

			std::istream *fs = m_resourcePacks->openFile(filePath);

			util::parseATN(*fs, list, false);

			if (list.maxID() + 1 > m_vacantUniqueID)
				m_vacantUniqueID = list.maxID() + 1;

			delete fs;
		}
		else if (filePath.substr(filePath.length() - 4, 4) == ".txt")
		{
			// We also reserve any desc IDs defined in text files,
			// as it's possible that some things aren't defined in the ERB
			StringList list;

			std::istream *fs = m_resourcePacks->openFile(filePath);

			*fs >> list;

			for (const std::string &key : list.keys())
			{
				if (key.length() >= 5 && list.isNumeric(key.substr(0, 5)))
				{
					int id = std::stoi(key.substr(0, 5));

					m_reservedDescIDs.emplace(id);
				}
			}

			delete fs;
		}

		numFilesProcessed++;

		if ((numFilesProcessed % 100) == 0)
		{
			mutex.progress = (numFilesProcessed / numFiles) * maxPerc;
			mutex.wait();
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

		[&](const std::pair<int, std::string> &lhs, const std::pair<int, std::string> &rhs)
	{
		if (lhs.first == rhs.first)
			return m_vacantDescIDs[lhs.second] < m_vacantDescIDs[rhs.second];
		else
			return lhs.first < rhs.first;
	}
	);
}

void ResourceMerger::prepareBaseDynamicFiles(Mutex::Server &mutex)
{
	mutex.queue.push("Restoring base game DynamicResources...\n");
	mutex.wait();

	char *buffer = new char[COPY_BUFFER_SIZE];

	std::streamsize bufferLength = 0;

	for (const std::pair<std::string, std::pair<std::string, std::string>> &keyPair : m_unpackedDynamicFiles)
	{
		const std::string &baseName = keyPair.second.first;
		const std::string &backupName = keyPair.second.second;

		std::ifstream backupFileIn(backupName);

		if (backupFileIn.good())
		{
			// Restore backup
			std::ofstream baseFile(baseName, std::ios::trunc);

			if (baseFile.fail())
			{
				throw ATN::Exception("Failed to open \"%s\" for writing (base dynamic)", baseName);
			}

			while (!backupFileIn.eof())
			{
				backupFileIn.read(buffer, COPY_BUFFER_SIZE);

				bufferLength = backupFileIn.gcount();

				baseFile.write(buffer, bufferLength);
			}
		}
		else
		{
			// Copy for backup

			std::ofstream backupFileOut(backupName, std::ios::trunc);

			if (backupFileOut.fail())
			{
				throw ATN::Exception("Failed to open \"%s\" for writing (backup dynamic)", backupName);
			}

			std::ifstream baseFile(baseName);

			if (baseFile.fail())
			{
				throw ATN::Exception("Failed to open \"%s\" for reading (base dynamic)", baseName);
			}

			while (!baseFile.eof())
			{
				baseFile.read(buffer, COPY_BUFFER_SIZE);

				bufferLength = baseFile.gcount();

				backupFileOut.write(buffer, bufferLength);
			}

			std::string filename = baseName.substr(baseName.find_last_of('/') + 1);

			mutex.queue.push("Backed up " + filename + "\n");
		}
	}

	mutex.queue.push("\n");

	delete[] buffer;
}

ResourceMerger::ResourceMerger(const ResourcePacks *packs, const std::string &outputFolder) : m_resourcePacks(packs), m_outputPath(outputFolder)
{

}

ResourceMerger::~ResourceMerger()
{
	delete m_resourcePacks;

	for (ModPack *mod : m_mods)
	{
		delete mod;
	}

	for (IResourcePatcher *patcher : m_patchers)
	{
		delete patcher;
	}
}

const ResourcePacks &ResourceMerger::resourcePacks()
{
	return *m_resourcePacks;
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

void ResourceMerger::addDynamicBaseFile(const std::string &backupFile, const std::string &baseFile)
{
	std::string filename = baseFile.substr(baseFile.find_last_of('/') + 1);

	m_unpackedDynamicFiles[filename] = std::make_pair(baseFile, backupFile);
}

bool ResourceMerger::isDynamicBaseFile(const std::string &filename) const
{
	return (m_unpackedDynamicFiles.find(filename) != m_unpackedDynamicFiles.end());
}

std::istream *ResourceMerger::openDynamicBaseFile(const std::string &filename) const
{
	std::ifstream *file = new std::ifstream(m_unpackedDynamicFiles.at(filename).second);

	if (file->fail())
		throw std::exception(("Couldn't open backup file \"" + filename + "\" for reading").c_str());

	return file;
}

void ResourceMerger::restoreBackups() const
{
	char *buffer = new char[COPY_BUFFER_SIZE];

	std::streamsize bufferLength = 0;

	for (const std::pair<std::string, std::pair<std::string, std::string>> &keyPair : m_unpackedDynamicFiles)
	{
		const std::string &baseName = keyPair.second.first;
		const std::string &backupName = keyPair.second.second;

		std::ifstream backupFileIn(backupName);

		if (backupFileIn.good())
		{
			// Restore backup
			std::ofstream baseFile(baseName, std::ios::trunc);

			if (baseFile.fail())
			{
				throw ATN::Exception("Failed to open \"%s\" for writing (base dynamic)", baseName);
			}

			while (!backupFileIn.eof())
			{
				backupFileIn.read(buffer, COPY_BUFFER_SIZE);

				bufferLength = backupFileIn.gcount();

				baseFile.write(buffer, bufferLength);
			}
		}
	}

	delete[] buffer;
}

void ResourceMerger::mergeMods(Mutex::Server &mutex)
{
	const std::string tabString("    ");

	double initPerc = 0.099;

	initialize(mutex, initPerc);

	mutex.progress = initPerc;
	mutex.wait();

	prepareBaseDynamicFiles(mutex);

	initPerc = 0.1;

	mutex.progress = initPerc;
	mutex.wait();

	int numModsProcessed = 0;

	for (ModPack *mod : m_mods)
	{
		mutex.queue.push("Installing \"" + mod->name() + "\":\n");

		std::vector<IResourcePatch*> patches;

		// Files are grouped by extension as the patcher may be dependent on several files (e.g. ATN global pointers)
		std::unordered_map<std::string, std::vector<std::string>> m_extFiles;

		double modProgress = 0;

		for (const std::string &fullFilePath : mod->files())
		{
			std::string fileExt = fullFilePath.substr(fullFilePath.find_last_of('.'));

			std::string filename = fullFilePath.substr(fullFilePath.find_last_of('/') + 1);

			// This file isn't permitted to be changed further
			if (m_lockedFiles.find(filename) != m_lockedFiles.end())
			{
				mutex.queue.push(tabString + "Not including \"" + filename + "\" due to conflicts\n");
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

		modProgress += 0.1;

		mutex.progress = initPerc + ((numModsProcessed + modProgress) / m_mods.size()) / (1 + initPerc);;

		mutex.wait();

		for (const std::pair<std::string, std::vector<std::string>> &pair : m_extFiles)
		{
			try
			{
				std::vector<IResourcePatch*> subPatches = m_extToPatch[pair.first]->createPatches(*this, *mod, pair.second);

				patches.insert(patches.end(), subPatches.begin(), subPatches.end());
			}
			catch (std::exception &e)
			{
				mutex.queue.push(tabString + tabString + "Failed to create patch for extension \"" + pair.first + "\":\n");
				mutex.queue.push(tabString + tabString + std::string(e.what()) + "\n");
			}
		}

		if (patches.size() == 1)
			mutex.queue.push("\n" + tabString + "Created 1 patch\n");
		else
			mutex.queue.push("\n" + tabString + "Created " + std::to_string(patches.size()) + " patches\n");

		modProgress += 0.5;

		mutex.progress = initPerc + ((numModsProcessed + modProgress) / m_mods.size()) / (1 + initPerc);;

		mutex.wait();

		for (IResourcePatch *patch : patches)
		{
			const std::vector<std::string> &filenames = patch->filenames();

			std::vector<std::istream*> inStreams;
			std::vector<std::ostream*> outStreams;

			bool bStreamsStable = true;

			for (const std::string &filename : filenames)
			{
				// For dynamic base files, we will always have an existing file to patch (which was restored earlier)
				if (isDynamicBaseFile(filename))
				{
					std::string baseName = m_unpackedDynamicFiles[filename].first;

					std::ifstream *fsBase = new std::ifstream(baseName);

					// fsIn is a copy of the original since we want to write back to the same file immediately
					std::stringstream *fsIn = new std::stringstream();

					char *buffer = new char[COPY_BUFFER_SIZE];

					std::streamsize bufferLength = 0;

					while (!fsBase->eof())
					{
						fsBase->read(buffer, COPY_BUFFER_SIZE);

						bufferLength = fsBase->gcount();

						fsIn->write(buffer, bufferLength);
					}

					delete fsBase;

					std::ofstream *fsOut = new std::ofstream(baseName, std::ios::trunc);

					if (fsOut->fail())
					{
						mutex.queue.push(tabString + tabString + "ERROR: Failed to open \"" + filename + "\" for writing (base dynamic)\n");

						bStreamsStable = false;
						break;
					}

					fsIn->seekg(0, std::ios::beg);

					inStreams.push_back(fsIn);
					outStreams.push_back(fsOut);

					delete[] buffer;
				}
				else
				{
					std::ifstream *fsManaged = new std::ifstream(m_outputPath + "/" + filename);

					// First occurence of this file
					if (fsManaged->fail())
					{
						delete fsManaged;

						std::ofstream *fsOut = new std::ofstream(m_outputPath + "/" + filename, std::ios::trunc);

						if (fsOut->fail())
						{
							mutex.queue.push(tabString + tabString + "ERROR: Failed to open \"" + filename + "\" for writing (managed)\n");

							delete fsOut;

							bStreamsStable = false;
							break;
						}

						if (m_resourcePacks->contains(filename))
						{
							// Patch from base files
							std::istream *fsBase = m_resourcePacks->openFile(filename);

							// Note: if file doesn't exist in base game, these won't be called
							inStreams.push_back(fsBase);
							outStreams.push_back(fsOut);
						}
						else
						{
							// This is a file that doesn't exist in the base game, apply directly

							std::vector<std::ostream*> directApplied;

							directApplied.push_back(fsOut);

							std::vector<std::string> strOutputs = patch->apply(*mod, directApplied);

							for (std::string &strOut : strOutputs)
							{
								mutex.queue.push(tabString + tabString + strOut + "\n");
							}

							delete fsOut;
						}
					}
					else
					{
						// Otherwise, we patch to an already patched file

						// fsIn is a copy of the original since we want to write back to the same file immediately
						std::stringstream *fsIn = new std::stringstream();

						char *buffer = new char[COPY_BUFFER_SIZE];

						std::streamsize bufferLength = 0;

						while (!fsManaged->eof())
						{
							fsManaged->read(buffer, COPY_BUFFER_SIZE);

							bufferLength = fsManaged->gcount();

							fsIn->write(buffer, bufferLength);
						}

						delete fsManaged;

						std::ofstream *fsOut = new std::ofstream(m_outputPath + "/" + filename, std::ios::trunc);

						if (fsOut->fail())
						{
							mutex.queue.push(tabString + tabString + "ERROR: Failed to open \"" + filename + "\" for writing (managed)\n");

							bStreamsStable = false;
							break;
						}

						fsIn->seekg(0, std::ios::beg);

						inStreams.push_back(fsIn);
						outStreams.push_back(fsOut);

						delete[] buffer;
					}
				}
			}

			if (bStreamsStable)
			{
				if (inStreams.size() > 0)
				{
					std::vector<std::string> strOutputs = patch->apply(inStreams, outStreams);

					for (std::string &strOut : strOutputs)
					{
						mutex.queue.push(tabString + tabString + strOut + "\n");
					}
				}
			}
			else
			{
				mutex.queue.push(tabString + tabString + "Patch not applied due to previous error!\n");
			}

			for (std::istream *stream : inStreams)
			{
				delete stream;
			}

			for (std::ostream *stream : outStreams)
			{
				delete stream;
			}

			delete patch;

			modProgress += 0.5 / patches.size();

			mutex.progress = initPerc + ((numModsProcessed + modProgress) / m_mods.size()) / (1 + initPerc);

			mutex.wait();
		}

		mutex.queue.push("\n" + tabString + "Patches applied\n\n\n");

		numModsProcessed++;
	}

	// In case this resource merger is used again, we need to clear any global references
	for (IResourcePatcher *patcher : m_patchers)
	{
		patcher->clearMemory();
	}

	mutex.queue.push("Merging complete");

	// Send last output texts
	mutex.progress = 1;
	mutex.wait();
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

bool ResourceMerger::isDescDefined(int descID)
{
	return (m_reservedDescIDs.find(descID) != m_reservedDescIDs.end());
}

int ResourceMerger::reserveDescID(const std::string &descClass)
{
	int cur = m_vacantDescIDs[descClass];

	// Scan for vacant positions (for some reason there may be some open slots between base definitions)
	while (m_reservedDescIDs.find(cur) != m_reservedDescIDs.end())
	{
		cur++;
	}

	if (this->descClass(cur) != descClass)
		throw ATN::Exception("Ran out of entity description IDs for class \"%s\"", descClass);

	m_vacantDescIDs[descClass] = cur + 1;

	return cur;
}

std::uint32_t ResourceMerger::reserveUniqueID()
{
	return m_vacantUniqueID++;
}

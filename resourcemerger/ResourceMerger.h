#pragma once

#include "IResourcePatcher.h"
#include "ModPack.h"
#include "ResourcePacks.h"

#include "ATN/ATN_Manager.h"

#include "MutexCommunication.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <queue>

// Forward declared
class IResourcePatch; class IResourcePatcher; class ModPack;

class ResourceMerger
{
protected:

	const ResourcePacks *m_resourcePacks = nullptr;

	std::vector<ModPack*> m_mods;

	std::vector<IResourcePatcher*> m_patchers;

	// Maps file extension to the patcher to use
	std::unordered_map<std::string, IResourcePatcher*> m_extToPatch;

	std::string m_outputPath;

	// A locked file should not be modified further
	std::unordered_set<std::string> m_lockedFiles;

	// A map containing a counter for the next unused desc ID in the given class
	std::unordered_map<std::string, int> m_vacantDescIDs;

	// A list of desc ID upper bound ranges and the class they belong to
	std::vector<std::pair<int, std::string>> m_descClassRanges;

	std::unordered_set<int> m_reservedDescIDs;

	std::uint32_t m_vacantUniqueID;

	// Buffer size to use when copying files
	const int COPY_BUFFER_SIZE = 4096;

	bool m_initialized = false;

	// Initializes known desc IDs and max unique IDs from the base game's files
	void initialize(Mutex::Server &mutex, double maxPerc);

public:

	ResourceMerger(const ResourcePacks *packs, const std::string &outputFolder);
	~ResourceMerger();

	// Access resources stored in any resource packs
	const ResourcePacks &resourcePacks();

	// Adds a new mod to be merged last
	void addMod(ModPack *modPack);

	// Adds a patcher for use in merging files
	void addPatcher(IResourcePatcher *patcher);

	// Merge the mods currently loaded in the merger, reporting progress to the given string queue
	void mergeMods(Mutex::Server &mutex);

	// Prevent a file from being modified by further patches
	void lockFile(const std::string &file);

	// Returns the entity description class that the given desc ID should belong to
	const std::string &descClass(int descID);

	// Returns whether or not the specified desc ID already exists in the base game
	bool isDescDefined(int descID);

	// Reserves a new desc ID for use in a mod
	int reserveDescID(const std::string &descClass);

	// Reserves a new ATN unique ID for use in a mod
	std::uint32_t reserveUniqueID();
};


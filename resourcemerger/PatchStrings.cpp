#include "PatchStrings.h"

PatchStrings::PatchStrings(const std::string &baseFilename, const std::string &modFilename) : IResourcePatch({ baseFilename }, { modFilename })
{

}

std::vector<std::string> PatchStrings::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	const std::string &filename = m_filenames[0];

	std::istream *in = inStreams[0];
	std::ostream *out = outStreams[0];

	StringList list;

	*in >> list;

	std::size_t numApplied = 0;

	for (const SubPatch &subPatch : m_subPatches)
	{
		switch (subPatch.m_type)
		{
		case ADD_INCLUDE:
		{
			bool bExists = false;

			// Already added by some other mod
			for (const std::string &includeFile : list.includes())
			{
				if (includeFile == subPatch.m_mod.first)
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				list.addInclude(subPatch.m_mod.first);

				numApplied++;
			}
		}
		break;
		case REM_INCLUDE:
		{
			list.removeInclude(subPatch.m_mod.first);

			numApplied++;
		}
		break;
		case ADD_ENTRY:
		{
			if (!list.contains(subPatch.m_mod.first))
			{
				list.set(subPatch.m_mod.first, subPatch.m_mod.second);

				numApplied++;
			}
		}
		break;
		case MOD_ENTRY:
		{
			if (list.contains(subPatch.m_base.first) && list.get(subPatch.m_base.first) == subPatch.m_base.second)
			{
				list.set(subPatch.m_mod.first, subPatch.m_mod.second);

				numApplied++;
			}
		}
		break;
		case REM_ENTRY:
		{
			if (list.contains(subPatch.m_base.first) && list.get(subPatch.m_base.first) == subPatch.m_base.second)
			{
				list.remove(subPatch.m_base.first);

				numApplied++;
			}
		}
		break;
		}
	}

	*out << list;

	if (numApplied != m_subPatches.size())
		strOutputs.push_back("Applied " + std::to_string(numApplied) + " of " + std::to_string(m_subPatches.size()) + " changes to " + filename);
	else if (numApplied != 1)
		strOutputs.push_back("Applied all " + std::to_string(numApplied) + " changes to " + filename);
	else
		strOutputs.push_back("Applied the 1 change to " + filename);

	return strOutputs;
}

std::vector<std::string> PatchStrings::apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	StringList list;

	const std::string &filename = m_filenames[0];

	std::ostream *out = outStreams[0];

	for (const SubPatch &subPatch : m_subPatches)
	{
		if (subPatch.m_type == ADD_INCLUDE)
		{
			list.addInclude(subPatch.m_mod.first);
		}
		else if (subPatch.m_type == ADD_ENTRY)
		{
			list.set(subPatch.m_mod.first, subPatch.m_mod.second);
		}
		else
		{
			strOutputs.push_back("First time writing " + filename + ", yet it's not an add patch...?");
		}
	}

	*out << list;

	strOutputs.push_back("Created " + filename);

	return strOutputs;
}

void PatchStrings::addSubPatch(SubPatch patch)
{
	m_subPatches.push_back(patch);
}

void PatcherStrings::updateDescReferences(ResourceMerger &merger, ModPack &mod, StringList &list)
{
	// List is copied so it won't be affected by sudden removals or additions
	std::vector<std::string> keys = list.keys();

	for (const std::string &key : keys)
	{
		if (key.length() < 5)
			continue;

		std::string strDescOriginal = key.substr(0, 5);

		if (!list.isNumeric(strDescOriginal))
			continue;

		int descID = std::stoi(strDescOriginal);

		descID = mod.translateDescID(descID, merger);

		char strDesc[6];

		snprintf(strDesc, sizeof(strDesc), "%05d", descID);

		if (strDescOriginal != strDesc)
		{
			std::string value = list.get(key);

			list.remove(key);
			list.set(strDesc + key.substr(5), value);
		}
	}
}

void PatcherStrings::buildPatch(ResourceMerger &merger, ModPack &mod, PatchStrings *patch, const StringList &baseList, const StringList &modList) const
{
	// Add new includes
	for (const std::string &modInclude : modList.includes())
	{
		bool bExists = false;

		for (const std::string &baseInclude : baseList.includes())
		{
			if (baseInclude == modInclude)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			patch->addSubPatch(PatchStrings::SubPatch{ PatchStrings::ADD_INCLUDE, std::make_pair(std::string(), std::string()), std::make_pair(modInclude, std::string()) });
		}
	}

	// Remove existing includes
	for (const std::string &baseInclude : baseList.includes())
	{
		bool bExists = false;

		for (const std::string &modInclude : modList.includes())
		{
			if (baseInclude == modInclude)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			patch->addSubPatch(PatchStrings::SubPatch{ PatchStrings::REM_INCLUDE, std::make_pair(baseInclude, std::string()), std::make_pair(std::string(), std::string()) });
		}
	}

	// Add new keys
	for (const std::string &modKey : modList.keys())
	{
		const std::string &modValue = modList.get(modKey);

		bool bExists = false;

		for (const std::string &baseKey : baseList.keys())
		{
			if (baseKey == modKey)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			patch->addSubPatch(PatchStrings::SubPatch{ PatchStrings::ADD_ENTRY, std::make_pair(std::string(), std::string()), std::make_pair(modKey, modValue) });
		}
	}

	// Remove existing keys
	for (const std::string &baseKey : baseList.keys())
	{
		const std::string &baseValue = baseList.get(baseKey);

		bool bExists = false;

		for (const std::string &modKey : modList.keys())
		{
			if (baseKey == modKey)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			patch->addSubPatch(PatchStrings::SubPatch{ PatchStrings::REM_ENTRY, std::make_pair(baseKey, baseValue), std::make_pair(std::string(), std::string()) });
		}
	}

	// Check for changed values
	for (const std::string &baseKey : baseList.keys())
	{
		const std::string &baseValue = baseList.get(baseKey);

		if (modList.contains(baseKey))
		{
			const std::string &modValue = modList.get(baseKey);

			if (baseValue != modValue)
			{
				patch->addSubPatch(PatchStrings::SubPatch{ PatchStrings::MOD_ENTRY, std::make_pair(baseKey, baseValue), std::make_pair(baseKey, modValue) });
			}
		}
	}
}

const char * const PatcherStrings::extension() const
{
	return ".txt";
}

std::vector<IResourcePatch*> PatcherStrings::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files)
{
	std::vector<IResourcePatch*> patches;

	std::unordered_set<std::string> excludedFiles;

	// These files aren't string key definitions, but may have
	// been modified so it's delegated to PatcherGeneric instead
	excludedFiles.emplace("AlertsToCheckScalesFreq.txt");
	excludedFiles.emplace("femalefirst.txt");
	excludedFiles.emplace("lastnames.txt");
	excludedFiles.emplace("malefirst.txt");
	excludedFiles.emplace("SpeakerLookup.txt");
	excludedFiles.emplace("teamnames.txt");
	excludedFiles.emplace("UserAllocationIds.txt");
	excludedFiles.emplace("teamnames.txt");

	for (const std::string &modFile : files)
	{
		std::string filename = modFile.substr(modFile.find_last_of('/') + 1);

		if (excludedFiles.find(filename) != excludedFiles.end())
		{
			std::vector<std::string> vFile;

			vFile.push_back(modFile);

			patches.push_back(PatcherGeneric(".txt", true).createPatches(merger, mod, vFile)[0]);
			continue;
		}

		PatchStrings *patch = new PatchStrings(filename, modFile);

		if (merger.resourcePacks().contains(filename))
		{
			std::istream *fsBase = merger.resourcePacks().openFile(filename);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			StringList baseList, modList;

			*fsBase >> baseList;

			fsMod >> modList;

			// If any key definition depends on a desc ID, we update it in case it was added by a mod
			updateDescReferences(merger, mod, modList);

			buildPatch(merger, mod, patch, baseList, modList);

			delete fsBase;
		}
		else
		{
			std::ifstream fs(modFile);

			if (fs.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			StringList modList;

			fs >> modList;

			updateDescReferences(merger, mod, modList);

			StringList emptyList;

			// Build subpatches in case multiple mods add the same string files
			buildPatch(merger, mod, patch, emptyList, modList);
		}

		patches.push_back(patch);
	}

	return patches;
}

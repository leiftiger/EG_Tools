#include "PatchStrings.h"

#include "utils.h"

void StringList::addInclude(const std::string &include)
{
	m_includes.push_back(include);
}

void StringList::removeInclude(const std::string &include)
{
	bool found = false;

	for (size_t i = 0; i < (m_includes.size() - 1); i++)
	{
		if (found || m_includes[i] == include)
		{
			found = true;
			m_includes[i] = m_includes[i + 1];
		}
	}

	if (found || m_includes.back() == include)
		m_includes.pop_back();
}

const std::vector<std::string> &StringList::includes() const
{
	return m_includes;
}

void StringList::set(const std::string &key, const std::string &value)
{
	if (m_map.find(key) == m_map.end())
	{
		m_keys.push_back(key);
	}

	m_map[key] = value;
}

const std::string &StringList::get(const std::string &key) const
{
	if (m_map.find(key) == m_map.end())
		throw std::exception(("Key not found: \"" + key + "\"").c_str());

	return m_map.at(key);
}

bool StringList::contains(const std::string &key) const
{
	return (m_map.find(key) != m_map.end());
}

void StringList::remove(const std::string &key)
{
	if (m_map.find(key) != m_map.end())
	{
		m_map.erase(key);

		for (int i = 0; i < m_keys.size(); i++)
		{
			std::string &curKey = m_keys[i];

			if (curKey == key)
			{
				for (int i2 = i; i2 < m_keys.size() - 1; i2++)
				{
					m_keys[i2] = m_keys[i2 + 1];
				}

				m_keys.pop_back();

				break;
			}
		}
	}
}


const std::vector<std::string> &StringList::keys() const
{
	return m_keys;
}

std::istream &operator>>(std::istream &stream, StringList &list)
{
	std::string line;

	std::string prevLine;

	bool tagOpen = false;

	std::string tagKey;
	std::string tagValue;

	while (util::getline(stream, line))
	{
		if (line.length() < 2)
		{
			prevLine = line;
			continue;
		}

		// Comment
		if (line.substr(0, 2) == "##")
		{
			prevLine = line;
			continue;
		}

		if (line.length() >= 8 && line.substr(0, 8) == "#include")
		{
			std::string filename = line.substr(strlen("#include "));

			list.addInclude(filename);
		}

		if (tagOpen)
		{
			if (line.length() >= 5 && line.substr(0, 5) == "<END>")
			{
				list.set(tagKey, tagValue);
				tagOpen = false;
			}
			else
			{
				tagValue += line + "\n";
			}
		}

		if (line.length() >= 7 && line.substr(0, 7) == "<BEGIN>")
		{
			tagOpen = true;
			tagKey = prevLine;
			tagValue = "";
		}

		prevLine = line;
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, const StringList &list)
{
	stream << "## File merged by Mod Manager" << std::endl;
	stream << "## Original comments can be found in the base or mod file" << std::endl;

	if (list.includes().size() > 0)
		stream << std::endl;

	for (const std::string &includeFile : list.includes())
	{
		stream << "#include " << includeFile << std::endl;
	}

	for (const std::string &key : list.keys())
	{
		const std::string &value = list.get(key);

		stream << std::endl;
		stream << key << std::endl;
		stream << "<BEGIN>" << std::endl;
		stream << value; // newline included in value
		stream << "<END>" << std::endl;
	}

	return stream;
}

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

	int numApplied = 0;

	for (const SubPatch &subPatch : m_subPatches)
	{
		switch (subPatch.m_type)
		{
		case ADD_INCLUDE:
		{
			list.addInclude(subPatch.m_mod.first);

			numApplied++;
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
			list.set(subPatch.m_mod.first, subPatch.m_mod.second);

			numApplied++;
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
	else
		strOutputs.push_back("Applied all " + std::to_string(numApplied) + " changes to " + filename);

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

bool PatcherStrings::isNumeric(const std::string &str) const
{
	for (char c : str)
	{
		if (!isdigit(c))
			return false;
	}

	return true;
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

		if (!isNumeric(strDescOriginal))
			continue;

		int descID = std::stoi(strDescOriginal);

		descID = mod.translateDescID(descID, merger);

		char strDesc[6];

		sprintf_s(strDesc, "%05d", descID);

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

		try
		{
			std::istream *fsBase = merger.resourcePacks().openFile(filename);

			PatchStrings *patch = new PatchStrings(filename, modFile);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::exception(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			StringList baseList, modList;

			*fsBase >> baseList;

			fsMod >> modList;

			// If any key definition depends on a desc ID, we update it in case it was added by a mod
			updateDescReferences(merger, mod, modList);

			buildPatch(merger, mod, patch, baseList, modList);

			delete fsBase;
		}
		catch (std::exception e)
		{
			std::ifstream fs(modFile);

			if (fs.fail())
			{
				throw std::exception(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			StringList modList;

			fs >> modList;

			updateDescReferences(merger, mod, modList);

			StringList emptyList;

			// Build subpatches in case multiple mods add the same string files
			buildPatch(merger, mod, patch, emptyList, modList);
		}

		patches.push_back(new PatchStrings(filename, modFile));
	}

	return patches;
}

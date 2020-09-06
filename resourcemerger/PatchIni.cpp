#include "PatchIni.h"

#include "utils.h"

IniList::IniList(char keyDelimiter) : m_keyDelimiter(keyDelimiter)
{

}

std::size_t IniList::size() const
{
	return m_keys.size();
}

void IniList::clear()
{
	m_keys.clear();
	m_values.clear();
}

void IniList::insert(std::size_t pos, const std::string &key, const std::string &value)
{
	m_keys.push_back(key);
	m_values.push_back(value);

	// If not inserted in the back, move all affected elements
	if (pos != (m_keys.size() - 1))
	{
		for (std::size_t i = m_keys.size() - 1; i > pos; i--)
		{
			m_keys[i] = m_keys[i - 1];
			m_values[i] = m_values[i - 1];
		}

		m_keys[pos] = key;
		m_values[pos] = value;
	}
}

void IniList::push_back(const std::string &key, const std::string &value)
{
	insert(m_keys.size(), key, value);
}

void IniList::set(std::size_t pos, const std::string &key, const std::string &value)
{
	m_keys[pos] = key;
	m_values[pos] = value;
}

void IniList::remove(std::size_t pos)
{
	for (std::size_t i = pos; i < m_keys.size() - 1; i++)
	{
		m_keys[i] = m_keys[i + 1];
		m_values[i] = m_values[i + 1];
	}

	m_keys.pop_back();
	m_values.pop_back();
}

void IniList::remove(const std::string &key, const std::string &value)
{
	for (std::size_t i = 0; i < m_keys.size(); i++)
	{
		if (m_keys[i] == key && m_values[i] == value)
		{
			remove(i);
			break;
		}
	}
}

const std::vector<std::string> &IniList::keys() const
{
	return m_keys;
}

const std::vector<std::string> &IniList::values() const
{
	return m_values;
}

bool IniList::getline(std::istream &stream, std::string &line)
{
	line = "";

	if (!stream.eof())
		m_newlineEOF = false;

	bool bReadSomething = false;

	while (!stream.eof())
	{
		char c = stream.get();

		bReadSomething = true;

		// Otherwise we seemingly read an invalid character
		if (stream.eof())
			break;

		if (c == '\n')
		{
			m_newlineEOF = true;
			return true;
		}
		else if (c != '\r')
		{
			line += c;
		}
	}

	if (bReadSomething)
		return true;

	return false;
}

std::istream &operator>>(std::istream &stream, IniList &list)
{
	list.clear();

	std::string line;

	while (list.getline(stream, line))
	{
		std::string key, value;

		int posEqual = (int)line.find_first_of(list.m_keyDelimiter);

		if (posEqual != -1)
		{
			key = line.substr(0, posEqual);
			value = line.substr(posEqual + 1);
		}
		else
		{
			key = line;
			value = "";
		}

		list.push_back(key, value);
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, const IniList &list)
{
	for (std::size_t i = 0; i < list.size(); i++)
	{
		const std::string &key = list.keys()[i];
		const std::string &value = list.values()[i];

		if (value == "")
		{
			stream << key;
		}
		else
		{
			stream << key << list.m_keyDelimiter << value;
		}

		// Preserve original newline EOF termination
		if (list.m_newlineEOF || (i < (list.size() - 1)))
		{
			stream << std::endl;
		}
	}

	return stream;
}

PatchIni::PatchIni(const std::string &baseFilename, const std::string &modFilename, PatcherIni &patcher, char keyDelimiter) : IResourcePatch({ baseFilename }, { modFilename }), m_patcher(patcher), m_keyDelimiter(keyDelimiter)
{

}

std::vector<std::string> PatchIni::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	std::istream *in = inStreams[0];
	std::ostream *out = outStreams[0];

	const std::string &filename = m_filenames[0];

	IniList list(m_keyDelimiter);

	*in >> list;

	std::vector<int> &baseTranslations = m_patcher.translations(filename);

	std::size_t numApplied = 0;

	for (const SubPatch &subPatch : m_subPatches)
	{
		int curIndex = baseTranslations[subPatch.m_pos];

		if (curIndex == -1)
		{
			continue;
		}

		switch (subPatch.m_type)
		{
		case ADD_ENTRY:
		{
			list.insert(curIndex + 1, subPatch.m_mod.first, subPatch.m_mod.second);

			m_patcher.updateTranslationsAfter(filename, curIndex, 1);

			numApplied++;
		}
		break;
		case REM_ENTRY:
		{
			if (list.keys()[curIndex] == subPatch.m_base.first && list.values()[curIndex] == subPatch.m_base.second)
			{
				list.remove(curIndex);

				baseTranslations[subPatch.m_pos] = -1;

				m_patcher.updateTranslationsAfter(filename, curIndex, -1);

				numApplied++;
			}
		}
		break;
		case MOD_ENTRY:
		case SUB_ENTRY:
		{
			if (list.keys()[curIndex] == subPatch.m_base.first && list.values()[curIndex] == subPatch.m_base.second)
			{
				list.set(curIndex, subPatch.m_mod.first, subPatch.m_mod.second);

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

void PatchIni::addSubPatch(SubPatch patch)
{
	m_subPatches.push_back(patch);
}

void PatcherIni::buildPatch(ResourceMerger &merger, ModPack &mod, PatchIni *patch, const IniList &baseList, const IniList &modList) const
{
	int **distance = new int*[baseList.size() + 1];

	for (std::size_t i = 0; i <= baseList.size(); i++)
	{
		distance[i] = new int[modList.size() + 1];
	}

	int **operation = new int*[baseList.size() + 1];

	for (std::size_t i = 0; i <= baseList.size(); i++)
	{
		operation[i] = new int[modList.size() + 1];
	}

	const int KEEP = 0;
	const int SUB = 1;
	const int ADD = 2;
	const int REM = 3;

	for (int i = 1; i <= (int)baseList.size(); i++)
	{
		distance[i][0] = i;
		operation[i][0] = ADD;
	}

	for (int i = 1; i <= (int)modList.size(); i++)
	{
		distance[0][i] = i;
		operation[0][i] = REM;
	}

	distance[0][0] = 0;
	operation[0][0] = KEEP;

	// We calculate edit distance for each line to find out which lines were added / removed
	for (std::size_t iBase = 1; iBase <= baseList.size(); iBase++)
	{
		for (std::size_t iMod = 1; iMod <= modList.size(); iMod++)
		{
			const std::string &baseKey = baseList.keys()[iBase - 1];
			const std::string &modKey = modList.keys()[iMod - 1];

			int dist = distance[iBase - 1][iMod - 1] + 2;

			int op = SUB;

			if (baseKey == modKey)
			{
				dist -= 2;
				op = KEEP;
			}

			if ((distance[iBase - 1][iMod] + 1) < dist)
			{
				dist = distance[iBase - 1][iMod] + 1;
				op = REM;
			}

			if ((distance[iBase][iMod - 1] + 1) < dist)
			{
				dist = distance[iBase][iMod - 1] + 1;
				op = ADD;
			}

			distance[iBase][iMod] = dist;
			operation[iBase][iMod] = op;
		}
	}

	int iBase = (int)baseList.size() - 1, iMod = (int)modList.size() - 1;

	while (iBase >= 0 && iMod >= 0)
	{
		const std::string &baseKey = baseList.keys()[iBase], &baseValue = baseList.values()[iBase];
		const std::string &modKey = modList.keys()[iMod], &modValue = modList.values()[iMod];

		int op = operation[iBase + 1][iMod + 1];

		switch (op)
		{
		case KEEP:
		{
			if (baseValue != modValue)
			{
				patch->addSubPatch(PatchIni::SubPatch{ PatchIni::MOD_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });
			}

			iBase--;
			iMod--;
		}
		break;
		case SUB:
		{
			patch->addSubPatch(PatchIni::SubPatch{ PatchIni::SUB_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });

			iBase--;
			iMod--;
		}
		break;
		case ADD:
		{
			patch->addSubPatch(PatchIni::SubPatch{ PatchIni::ADD_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });

			iMod--;
		}
		break;
		case REM:
		{
			patch->addSubPatch(PatchIni::SubPatch{ PatchIni::REM_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });

			iBase--;
		}
		break;
		}
	}

	if (iBase != -1)
	{
		for (int i = iBase; i > -1; i--)
		{
			const std::string &baseKey = baseList.keys()[i], &baseValue = baseList.values()[i];

			patch->addSubPatch(PatchIni::SubPatch{ PatchIni::REM_ENTRY, i, std::make_pair(baseKey, baseValue), std::make_pair(baseKey, baseValue) });
		}
	}

	if (iMod != -1)
	{

		for (int i = iMod; i > -1; i--)
		{
			const std::string &modKey = modList.keys()[i], &modValue = modList.values()[i];

			patch->addSubPatch(PatchIni::SubPatch{ PatchIni::ADD_ENTRY, 0, std::make_pair(modKey, modValue), std::make_pair(modKey, modValue) });
		}
	}

	for (std::size_t i = 0; i <= baseList.size(); i++)
	{
		delete[] distance[i];
	}

	for (std::size_t i = 0; i <= baseList.size(); i++)
	{
		delete[] operation[i];
	}

	delete[] distance;
	delete[] operation;
}

std::vector<int> &PatcherIni::translations(const std::string &file)
{
	if (m_baseTranslations.find(file) == m_baseTranslations.end())
	{
		throw std::runtime_error(("Translation list not initialized for " + file).c_str());
	}

	return m_baseTranslations.at(file);
}

void PatcherIni::updateTranslationsAfter(const std::string &file, int index, int diff)
{
	std::vector<int> &baseTranslations = translations(file);

	for (int i = index + 1; i < (int)baseTranslations.size(); i++)
	{
		if (baseTranslations[i] != -1)
			baseTranslations[i] += diff;
	}
}

const char * const PatcherIni::extension() const
{
	return ".ini";
}

std::vector<IResourcePatch*> PatcherIni::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files)
{
	std::vector<IResourcePatch*> patches;

	for (const std::string &modFile : files)
	{
		std::string filename = modFile.substr(modFile.find_last_of('/') + 1);

		char delimiter = ' ';

		if (merger.isDynamicBaseFile(filename))
		{
			// Currently only supports updating this file for now
			if (filename != "ObjectiveDetails.ini")
			{
				std::vector<std::string> vFile;

				vFile.push_back(modFile);

				patches.push_back(PatcherGeneric(".ini", false).createPatches(merger, mod, vFile)[0]);
				continue;
			}

			std::istream *fsBase = merger.openDynamicBaseFile(filename);

			PatchIni *patch = new PatchIni(filename, modFile, *this, delimiter);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			IniList listBase(delimiter), listMod(delimiter);

			*fsBase >> listBase;

			fsMod >> listMod;

			// Initialize base translations that later will be modified
			std::vector<int> vecTranslations;

			for (int i = 0; i < (int)listBase.size(); i++)
			{
				vecTranslations.push_back(i);
			}

			m_baseTranslations[filename] = vecTranslations;

			buildPatch(merger, mod, patch, listBase, listMod);

			patches.push_back(patch);

			delete fsBase;
		}
		else
		{
			// Divert non-dynamic files to generic patcher
			std::vector<std::string> vFile;

			vFile.push_back(modFile);

			patches.push_back(PatcherGeneric(".ini", false).createPatches(merger, mod, vFile)[0]);
			continue;
		}
	}

	return patches;
}

void PatcherIni::clearMemory()
{
	m_baseTranslations.clear();
}

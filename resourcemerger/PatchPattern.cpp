#include "PatchPattern.h"

#include "utils.h"

#include <regex>

std::size_t EntityPattern::size() const
{
	return m_keys.size();
}

void EntityPattern::clear()
{
	m_keys.clear();
	m_values.clear();
}

void EntityPattern::insert(std::size_t pos, const std::string &key, const std::string &value)
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

void EntityPattern::push_back(const std::string &key, const std::string &value)
{
	insert(m_keys.size(), key, value);
}

void EntityPattern::set(std::size_t pos, const std::string &key, const std::string &value)
{
	m_keys[pos] = key;
	m_values[pos] = value;
}

void EntityPattern::remove(std::size_t pos)
{
	for (std::size_t i = pos; i < m_keys.size() - 1; i++)
	{
		m_keys[i] = m_keys[i + 1];
		m_values[i] = m_values[i + 1];
	}

	m_keys.pop_back();
	m_values.pop_back();
}

void EntityPattern::remove(const std::string &key, const std::string &value)
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

const std::vector<std::string> &EntityPattern::keys() const
{
	return m_keys;
}

const std::vector<std::string> &EntityPattern::values() const
{
	return m_values;
}

bool EntityPattern::getline(std::istream &stream, std::string &line)
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

std::istream &operator>>(std::istream &stream, EntityPattern &desc)
{
	desc.clear();

	std::string line;

	while (desc.getline(stream, line))
	{
		std::string key, value;

		int posEqual = (int)line.find_first_of('=');

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

		desc.push_back(key, value);
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, const EntityPattern &desc)
{
	for (std::size_t i = 0; i < desc.size(); i++)
	{
		const std::string &key = desc.keys()[i];
		const std::string &value = desc.values()[i];

		if (value == "")
		{
			stream << key;
		}
		else
		{
			stream << key << "=" << value;
		}

		// Preserve original newline EOF termination
		if (desc.m_newlineEOF || (i < (desc.size() - 1)))
		{
			stream << std::endl;
		}
	}

	return stream;
}

PatchPattern::PatchPattern(const std::string &outFilename, const std::string &modFilename, PatcherPattern &patcher) : IResourcePatch({ outFilename }, { modFilename }), m_patcher(patcher)
{

}

std::vector<std::string> PatchPattern::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	std::istream *in = inStreams[0];
	std::ostream *out = outStreams[0];

	const std::string &filename = m_filenames[0];

	EntityPattern desc;

	*in >> desc;

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
			desc.insert(curIndex + 1, subPatch.m_mod.first, subPatch.m_mod.second);

			m_patcher.updateTranslationsAfter(filename, curIndex, 1);

			numApplied++;
		}
		break;
		case REM_ENTRY:
		{
			if (desc.keys()[curIndex] == subPatch.m_base.first && desc.values()[curIndex] == subPatch.m_base.second)
			{
				desc.remove(curIndex);

				baseTranslations[subPatch.m_pos] = -1;

				m_patcher.updateTranslationsAfter(filename, curIndex, -1);

				numApplied++;
			}
		}
		break;
		case MOD_ENTRY:
		case SUB_ENTRY:
		{
			if (desc.keys()[curIndex] == subPatch.m_base.first && desc.values()[curIndex] == subPatch.m_base.second)
			{
				desc.set(curIndex, subPatch.m_mod.first, subPatch.m_mod.second);

				numApplied++;
			}
		}
		break;
		}
	}

	*out << desc;

	if (numApplied != m_subPatches.size())
		strOutputs.push_back("Applied " + std::to_string(numApplied) + " of " + std::to_string(m_subPatches.size()) + " changes to " + filename);
	else if (numApplied != 1)
		strOutputs.push_back("Applied all " + std::to_string(numApplied) + " changes to " + filename);
	else
		strOutputs.push_back("Applied the 1 change to " + filename);

	return strOutputs;
}

std::vector<std::string> PatchPattern::apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	const std::string &modFilename = m_modFiles[0];

	std::ostream *out = outStreams[0];

	std::istream *modFile = mod.openFile(modFilename);

	if (modFile->fail())
	{
		strOutputs.push_back("Couldn't open \"" + modFilename + "\" for reading (mod)");

		return strOutputs;
	}

	EntityPattern desc;

	*modFile >> desc;

	// Process the file for any desc ID references that may have to be updated
	for (std::size_t i = 0; i < desc.size(); i++)
	{
		const std::string &key = desc.keys()[i], &value = desc.values()[i];

		if (key == "OverlayPattern")
		{
			std::regex matchExpression("(.+for[\t ]*)([0-9]+)");
			std::smatch matches;

			if (std::regex_search(value, matches, matchExpression))
			{
				int descID = std::stoi(matches[2]);

				desc.set(i, key, matches[1].str() + std::to_string(mod.translateDescID(descID)));
			}
		}
	}

	*out << desc;

	delete modFile;

	strOutputs.push_back("Processed " + m_filenames[0]);

	return strOutputs;
}

void PatchPattern::addSubPatch(SubPatch patch)
{
	m_subPatches.push_back(patch);
}

void PatcherPattern::buildPatch(ResourceMerger &merger, ModPack &mod, PatchPattern *patch, const EntityPattern &baseDesc, const EntityPattern &modDesc) const
{
	int **distance = new int*[baseDesc.size() + 1];

	for (std::size_t i = 0; i <= baseDesc.size(); i++)
	{
		distance[i] = new int[modDesc.size() + 1];
	}

	int **operation = new int*[baseDesc.size() + 1];

	for (std::size_t i = 0; i <= baseDesc.size(); i++)
	{
		operation[i] = new int[modDesc.size() + 1];
	}

	const int KEEP = 0;
	const int SUB = 1;
	const int ADD = 2;
	const int REM = 3;

	for (int i = 1; i <= (int)baseDesc.size(); i++)
	{
		distance[i][0] = i;
		operation[i][0] = ADD;
	}

	for (int i = 1; i <= (int)modDesc.size(); i++)
	{
		distance[0][i] = i;
		operation[0][i] = REM;
	}

	distance[0][0] = 0;
	operation[0][0] = KEEP;

	// We calculate edit distance for each line to find out which lines were added / removed
	for (std::size_t iBase = 1; iBase <= baseDesc.size(); iBase++)
	{
		for (std::size_t iMod = 1; iMod <= modDesc.size(); iMod++)
		{
			const std::string &baseKey = baseDesc.keys()[iBase - 1];
			const std::string &modKey = modDesc.keys()[iMod - 1];

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

	int iBase = (int)baseDesc.size() - 1, iMod = (int)modDesc.size() - 1;

	while (iBase >= 0 && iMod >= 0)
	{
		const std::string &baseKey = baseDesc.keys()[iBase], &baseValue = baseDesc.values()[iBase];
		const std::string &modKey = modDesc.keys()[iMod], &modValue = modDesc.values()[iMod];

		int op = operation[iBase + 1][iMod + 1];

		switch (op)
		{
		case KEEP:
		{
			if (baseValue != modValue)
			{
				patch->addSubPatch(PatchPattern::SubPatch{ PatchPattern::MOD_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });
			}

			iBase--;
			iMod--;
		}
		break;
		case SUB:
		{
			patch->addSubPatch(PatchPattern::SubPatch{ PatchPattern::SUB_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });

			iBase--;
			iMod--;
		}
		break;
		case ADD:
		{
			patch->addSubPatch(PatchPattern::SubPatch{ PatchPattern::ADD_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });

			iMod--;
		}
		break;
		case REM:
		{
			patch->addSubPatch(PatchPattern::SubPatch{ PatchPattern::REM_ENTRY, iBase, std::make_pair(baseKey, baseValue), std::make_pair(modKey, modValue) });

			iBase--;
		}
		break;
		}
	}

	if (iBase != -1)
	{
		for (int i = iBase; i > -1; i--)
		{
			const std::string &baseKey = baseDesc.keys()[i], &baseValue = baseDesc.values()[i];

			patch->addSubPatch(PatchPattern::SubPatch{ PatchPattern::REM_ENTRY, i, std::make_pair(baseKey, baseValue), std::make_pair(baseKey, baseValue) });
		}
	}

	if (iMod != -1)
	{

		for (int i = iMod; i > -1; i--)
		{
			const std::string &modKey = modDesc.keys()[i], &modValue = modDesc.values()[i];

			patch->addSubPatch(PatchPattern::SubPatch{ PatchPattern::ADD_ENTRY, 0, std::make_pair(modKey, modValue), std::make_pair(modKey, modValue) });
		}
	}

	for (std::size_t i = 0; i <= baseDesc.size(); i++)
	{
		delete[] distance[i];
	}

	for (std::size_t i = 0; i <= baseDesc.size(); i++)
	{
		delete[] operation[i];
	}

	delete[] distance;
	delete[] operation;
}

std::vector<int> &PatcherPattern::translations(const std::string &file)
{
	if (m_baseTranslations.find(file) == m_baseTranslations.end())
	{
		throw std::runtime_error(("Translation list not initialized for " + file).c_str());
	}

	return m_baseTranslations.at(file);
}

void PatcherPattern::updateTranslationsAfter(const std::string &file, int index, int diff)
{
	std::vector<int> &baseTranslations = translations(file);

	for (int i = index + 1; i < (int)baseTranslations.size(); i++)
	{
		if (baseTranslations[i] != -1)
			baseTranslations[i] += diff;
	}
}

const char * const PatcherPattern::extension() const
{
	return ".pat";
}

std::vector<IResourcePatch*> PatcherPattern::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files)
{
	std::vector<IResourcePatch*> patches;

	for (const std::string &modFile : files)
	{
		std::string filename = modFile.substr(modFile.find_last_of('/') + 1);

		if (merger.resourcePacks().contains(filename))
		{
			std::istream *fsBase = merger.resourcePacks().openFile(filename);

			PatchPattern *patch = new PatchPattern(filename, modFile, *this);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			EntityPattern descBase, descMod;

			*fsBase >> descBase;

			fsMod >> descMod;

			// Initialize base translations that later will be modified
			std::vector<int> vecTranslations;

			for (int i = 0; i < (int)descBase.size(); i++)
			{
				vecTranslations.push_back(i);
			}

			m_baseTranslations[filename] = vecTranslations;

			// Process the file for any desc ID references that may have to be updated
			for (std::size_t i = 0; i < descMod.size(); i++)
			{
				const std::string &key = descMod.keys()[i], &value = descMod.values()[i];

				if (key == "OverlayPattern")
				{
					std::regex matchExpression("(.+for[\t ]*)([0-9]+)");
					std::smatch matches;

					if (std::regex_search(value, matches, matchExpression))
					{
						int descID = std::stoi(matches[2]);

						descMod.set(i, key, matches[1].str() + std::to_string(mod.translateDescID(descID, merger)));
					}
				}
			}

			buildPatch(merger, mod, patch, descBase, descMod);

			patches.push_back(patch);

			delete fsBase;
		}
		else
		{
			// New addition, we only need to update references
			std::ifstream fs(modFile);

			if (fs.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			EntityPattern desc;

			fs >> desc;

			// Process the file for any desc ID references that may have to be updated
			// The purpose here is not to change anything being written, but to make sure references are updated (since elsewhere we don't have access to the merger)
			for (std::size_t i = 0; i < desc.size(); i++)
			{
				const std::string &key = desc.keys()[i], &value = desc.values()[i];

				if (key == "OverlayPattern")
				{
					std::regex matchExpression("(.+for[\t ]*)([0-9]+)");
					std::smatch matches;

					if (std::regex_search(value, matches, matchExpression))
					{
						int descID = std::stoi(matches[2]);

						desc.set(i, key, matches[1].str() + std::to_string(mod.translateDescID(descID, merger)));
					}
				}
			}

			// Since the file won't possibly conflict with anything, we won't need any subpatching
			patches.push_back(new PatchPattern(filename, modFile, *this));
		}
	}

	return patches;
}

void PatcherPattern::clearMemory()
{
	m_baseTranslations.clear();
}

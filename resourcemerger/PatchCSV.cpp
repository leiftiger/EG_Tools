#include "PatchCSV.h"

#include "utils.h"

std::size_t SpreadsheetCSV::size() const
{
	return m_rows.size();
}

void SpreadsheetCSV::clear()
{
	m_headers.clear();
	m_rows.clear();

	m_keys.clear();
}

void SpreadsheetCSV::insertRow(std::size_t pos, std::vector<std::string> row)
{
	if (hasKey(row[0]))
		throw std::runtime_error(("Primary key already exists, cannot add \"" + row[0] + "\"").c_str());

	m_keys.emplace(row[0]);

	m_rows.push_back(row);

	// If not inserted in the back, move all affected elements
	if (pos != (m_rows.size() - 1))
	{
		for (size_t i = m_rows.size() - 1; i > pos; i--)
		{
			m_rows[i] = m_rows[i - 1];
		}

		m_rows[pos] = row;
	}
}

void SpreadsheetCSV::addRow(std::vector<std::string> row)
{
	insertRow(m_rows.size(), row);
}

void SpreadsheetCSV::removeRow(std::size_t pos)
{
	m_keys.erase(m_rows[pos][0]);

	for (size_t i = pos; i < m_rows.size() - 1; i++)
	{
		m_rows[i] = m_rows[i + 1];
	}

	m_rows.pop_back();
}

bool SpreadsheetCSV::hasKey(const std::string &key) const
{
	return (m_headers[0] == "DescID" && m_keys.find(key) != m_keys.end());
}

void SpreadsheetCSV::changeKey(const std::string &original, const std::string &change)
{
	m_keys.erase(original);
	m_keys.emplace(change);
}

const std::vector<std::string> &SpreadsheetCSV::headers() const
{
	return m_headers;
}

std::vector<std::string> &SpreadsheetCSV::row(std::size_t pos)
{
	return m_rows[pos];
}

const std::vector<std::string> &SpreadsheetCSV::row(std::size_t pos) const
{
	return m_rows[pos];
}

std::vector<std::string> SpreadsheetCSV::splitString(std::string str, char delimiter)
{
	std::vector<std::string> strList;

	int curStart = 0;

	for (int i = 0; i < (int)str.length(); i++)
	{
		if (str[i] == delimiter)
		{
			strList.push_back(str.substr(curStart, i - curStart));

			curStart = i + 1;
		}
	}

	// This is fine even if we're at the end of the string, because the last item in a row could potentially be empty too
	strList.push_back(str.substr(curStart));

	return strList;
}

std::string SpreadsheetCSV::concatString(const std::vector<std::string> &strList, char delimiter)
{
	std::string concatString;

	for (std::size_t i = 0; i < strList.size(); i++)
	{
		if (i > 0)
			concatString += delimiter;

		concatString += strList[i];
	}

	return concatString;
}


std::istream &operator>>(std::istream &stream, SpreadsheetCSV &list)
{
	list.clear();

	std::string line;

	while (util::getline(stream, line))
	{
		// Comment or blank line
		if (line.size() == 0 || line[0] == '#')
			continue;

		std::vector<std::string> row = list.splitString(line, ',');

		// Assume that the first row seen contains the headers (only fails for Glossary.csv which for some reason commented the headers)
		if (list.m_headers.size() == 0)
			list.m_headers = row;
		else
			list.addRow(row);
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream, const SpreadsheetCSV &list)
{
	stream << list.concatString(list.headers(), ',') << std::endl << std::endl;

	for (std::size_t i = 0; i < list.size(); i++)
	{
		stream << list.concatString(list.row(i), ',') << std::endl;
	}

	return stream;
}

PatchCSV::PatchCSV(const std::string &outFilename, const std::string &modFilename, PatcherCSV &patcher) : IResourcePatch({ outFilename }, { modFilename }), m_patcher(patcher)
{

}

std::vector<std::string> PatchCSV::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	std::istream *in = inStreams[0];
	std::ostream *out = outStreams[0];

	const std::string &filename = m_filenames[0];

	SpreadsheetCSV list;

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
			// Prevent duplicate entries in cases where the CSV contains a primary key
			if (!list.hasKey(subPatch.m_mod[0]))
			{
				list.insertRow(curIndex + 1, subPatch.m_mod);

				m_patcher.updateTranslationsAfter(filename, curIndex, 1);

				numApplied++;
			}
		}
		break;
		case REM_ENTRY:
		{
			if (list.row(curIndex) == subPatch.m_base)
			{
				list.removeRow(curIndex);

				baseTranslations[subPatch.m_pos] = -1;

				m_patcher.updateTranslationsAfter(filename, curIndex, -1);

				numApplied++;
			}
		}
		break;
		case MOD_ENTRY:
		case SUB_ENTRY:
		{
			// TODO: Only update affected columns
			if (list.row(curIndex) == subPatch.m_base)
			{
				std::string originalKey = list.row(curIndex)[0];
				std::string changedKey = subPatch.m_base[0];

				list.row(curIndex) = subPatch.m_mod;

				// Update primary keys due to substituing an entire row
				if (originalKey != changedKey)
					list.changeKey(originalKey, changedKey);

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

void PatchCSV::addSubPatch(SubPatch patch)
{
	m_subPatches.push_back(patch);
}

void PatcherCSV::buildPatch(ResourceMerger &merger, ModPack &mod, PatchCSV *patch, const SpreadsheetCSV &baseList, const SpreadsheetCSV &modList) const
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
			bool equal = false;

			if (baseList.headers()[0] == "DescID")
			{
				const std::string &baseKey = baseList.row(iBase - 1)[0];
				const std::string &modKey = modList.row(iMod - 1)[0];

				equal = (baseKey == modKey);
			}
			else
			{
				equal = (baseList.row(iBase - 1) == modList.row(iMod - 1));
			}

			int dist = distance[iBase - 1][iMod - 1] + 2;

			int op = SUB;

			if (equal)
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
		const std::vector<std::string> &baseRow = baseList.row(iBase);
		const std::vector<std::string> &modRow = modList.row(iMod);

		int op = operation[iBase + 1][iMod + 1];

		switch (op)
		{
		case KEEP:
		{
			if (baseRow != modRow)
			{
				patch->addSubPatch(PatchCSV::SubPatch{ PatchCSV::MOD_ENTRY, iBase, baseRow, modRow });
			}

			iBase--;
			iMod--;
		}
		break;
		case SUB:
		{
			patch->addSubPatch(PatchCSV::SubPatch{ PatchCSV::SUB_ENTRY, iBase, baseRow, modRow });

			iBase--;
			iMod--;
		}
		break;
		case ADD:
		{
			patch->addSubPatch(PatchCSV::SubPatch{ PatchCSV::ADD_ENTRY, iBase, {}, modRow });

			iMod--;
		}
		break;
		case REM:
		{
			patch->addSubPatch(PatchCSV::SubPatch{ PatchCSV::REM_ENTRY, iBase, baseRow, {} });

			iBase--;
		}
		break;
		}
	}

	if (iBase != -1)
	{
		for (int i = iBase; i > -1; i--)
		{
			const std::vector<std::string> &baseRow = baseList.row(i);

			patch->addSubPatch(PatchCSV::SubPatch{ PatchCSV::REM_ENTRY, i, baseRow, {} });
		}
	}

	if (iMod != -1)
	{

		for (int i = iMod; i > -1; i--)
		{
			const std::vector<std::string> &modRow = modList.row(iMod);

			patch->addSubPatch(PatchCSV::SubPatch{ PatchCSV::ADD_ENTRY, 0, {}, modRow });
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

std::vector<int> &PatcherCSV::translations(const std::string &file)
{
	if (m_baseTranslations.find(file) == m_baseTranslations.end())
	{
		throw std::runtime_error(("Translation list not initialized for " + file).c_str());
	}

	return m_baseTranslations.at(file);
}

void PatcherCSV::updateTranslationsAfter(const std::string &file, int index, int diff)
{
	std::vector<int> &baseTranslations = translations(file);

	for (int i = index + 1; i < (int)baseTranslations.size(); i++)
	{
		if (baseTranslations[i] != -1)
			baseTranslations[i] += diff;
	}
}

void PatcherCSV::updateDescReferences(ResourceMerger &merger, ModPack &mod, SpreadsheetCSV &list)
{
	for (std::size_t iCol = 0; iCol < list.headers().size(); iCol++)
	{
		const std::string &header = list.headers()[iCol];

		if (header == "DescID" || header == "PotentialFollower" || header == "Subject" || header == "Object 1" || header == "Object 2" || header == "Object 3" || header == "UnlockID")
		{
			for (std::size_t iRow = 0; iRow < list.size(); iRow++)
			{
				std::string strList = list.row(iRow)[iCol];

				// Some columns may be blank
				if (strList.length() > 0)
				{
					int oldDescID = std::stoi(strList);

					int newDescID = mod.translateDescID(oldDescID, merger);

					if (oldDescID != newDescID)
					{
						list.row(iRow)[iCol] = std::to_string(newDescID);
					}
				}
			}
		}
	}
}

const char * const PatcherCSV::extension() const
{
	return ".csv";
}

std::vector<IResourcePatch*> PatcherCSV::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files)
{
	std::vector<IResourcePatch*> patches;

	for (const std::string &modFile : files)
	{
		std::string filename = modFile.substr(modFile.find_last_of('/') + 1);

		if (merger.isDynamicBaseFile(filename))
		{
			std::istream *fsBase = merger.openDynamicBaseFile(filename);

			PatchCSV *patch = new PatchCSV(filename, modFile, *this);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			SpreadsheetCSV listBase, listMod;

			*fsBase >> listBase;

			fsMod >> listMod;

			// Initialize base translations that later will be modified
			std::vector<int> vecTranslations;

			for (int i = 0; i < (int)listBase.size(); i++)
			{
				vecTranslations.push_back(i);
			}

			m_baseTranslations[filename] = vecTranslations;

			// Process the file for any desc ID references that may have to be updated
			updateDescReferences(merger, mod, listMod);

			buildPatch(merger, mod, patch, listBase, listMod);

			patches.push_back(patch);

			delete fsBase;
		}
		else if (merger.resourcePacks().contains(filename))
		{
			std::istream *fsBase = merger.resourcePacks().openFile(filename);

			PatchCSV *patch = new PatchCSV(filename, modFile, *this);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::runtime_error(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			SpreadsheetCSV listBase, listMod;

			*fsBase >> listBase;

			fsMod >> listMod;

			// Initialize base translations that later will be modified
			std::vector<int> vecTranslations;

			for (int i = 0; i < (int)listBase.size(); i++)
			{
				vecTranslations.push_back(i);
			}

			m_baseTranslations[filename] = vecTranslations;

			// Process the file for any desc ID references that may have to be updated
			updateDescReferences(merger, mod, listMod);

			buildPatch(merger, mod, patch, listBase, listMod);

			patches.push_back(patch);

			delete fsBase;
		}
	}

	return patches;
}

void PatcherCSV::clearMemory()
{
	m_baseTranslations.clear();
}

#include "PatchGeneric.h"

void PatchGeneric::insert(std::size_t pos, const std::string &line, std::vector<std::string> &file) const
{
	file.push_back(line);

	// If not inserted in the back, move all affected elements
	if (pos != (file.size() - 1))
	{
		for (size_t i = file.size() - 1; i > pos; i--)
		{
			file[i] = file[i - 1];
		}

		file[pos] = line;
	}
}

void PatchGeneric::remove(std::size_t pos, std::vector<std::string> &file) const
{
	for (size_t i = pos; i < file.size() - 1; i++)
	{
		file[i] = file[i + 1];
	}

	file.pop_back();
}

PatchGeneric::PatchGeneric(const std::string &outFilename, const std::string &modFilename, PatcherGeneric &patcher) : IResourcePatch({ outFilename }, { modFilename }), m_patcher(patcher)
{

}

std::vector<std::string> PatchGeneric::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> strOutputs;

	std::istream *in = inStreams[0];
	std::ostream *out = outStreams[0];

	const std::string &filename = m_filenames[0];

	std::vector<std::string> file = m_patcher.loadFile(*in);

	std::vector<int> &baseTranslations = m_patcher.translations(filename);

	int numApplied = 0;

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
			insert(curIndex + 1, subPatch.m_mod, file);

			m_patcher.updateTranslationsAfter(filename, curIndex, 1);

			numApplied++;
		}
		break;
		case REM_ENTRY:
		{
			if (file[curIndex] == subPatch.m_base)
			{
				remove(curIndex, file);

				baseTranslations[subPatch.m_pos] = -1;

				m_patcher.updateTranslationsAfter(filename, curIndex, -1);

				numApplied++;
			}
		}
		break;
		case MOD_ENTRY:
		case SUB_ENTRY:
		{
			if (file[curIndex] == subPatch.m_base)
			{
				file[curIndex] = subPatch.m_mod;

				numApplied++;
			}
		}
		break;
		}
	}

	m_patcher.writeFile(*out, file);

	if (numApplied != m_subPatches.size())
		strOutputs.push_back("Applied " + std::to_string(numApplied) + " of " + std::to_string(m_subPatches.size()) + " changes to " + filename);
	else if (numApplied != 1)
		strOutputs.push_back("Applied all " + std::to_string(numApplied) + " changes to " + filename);
	else
		strOutputs.push_back("Applied the 1 change to " + filename);

	return strOutputs;
}

void PatchGeneric::addSubPatch(SubPatch patch)
{
	m_subPatches.push_back(patch);
}

void PatcherGeneric::buildPatch(ResourceMerger &merger, ModPack &mod, PatchGeneric *patch, const std::vector<std::string> &baseFile, const std::vector<std::string> &modFile) const
{
	int **distance = new int*[baseFile.size() + 1];

	for (int i = 0; i <= baseFile.size(); i++)
	{
		distance[i] = new int[modFile.size() + 1];
	}

	int **operation = new int*[baseFile.size() + 1];

	for (int i = 0; i <= baseFile.size(); i++)
	{
		operation[i] = new int[modFile.size() + 1];
	}

	const int KEEP = 0;
	const int SUB = 1;
	const int ADD = 2;
	const int REM = 3;

	for (int i = 1; i <= baseFile.size(); i++)
	{
		distance[i][0] = i;
		operation[i][0] = ADD;
	}

	for (int i = 1; i <= modFile.size(); i++)
	{
		distance[0][i] = i;
		operation[0][i] = REM;
	}

	distance[0][0] = 0;
	operation[0][0] = KEEP;

	// We calculate edit distance for each line to find out which lines were added / removed
	for (int iBase = 1; iBase <= baseFile.size(); iBase++)
	{
		for (int iMod = 1; iMod <= modFile.size(); iMod++)
		{
			const std::string &baseLine = baseFile[iBase - 1];
			const std::string &modLine = modFile[iMod - 1];

			int dist = distance[iBase - 1][iMod - 1] + 2;

			int op = SUB;

			if (baseLine == modLine)
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

	int iBase = (int)baseFile.size() - 1, iMod = (int)modFile.size() - 1;

	while (iBase >= 0 && iMod >= 0)
	{
		const std::string &baseLine = baseFile[iBase];
		const std::string &modLine = modFile[iMod];

		int op = operation[iBase + 1][iMod + 1];

		switch (op)
		{
		case KEEP:
		{
			if (baseLine != modLine)
			{
				patch->addSubPatch(PatchGeneric::SubPatch{ PatchGeneric::MOD_ENTRY, iBase, baseLine, modLine });
			}

			iBase--;
			iMod--;
		}
		break;
		case SUB:
		{
			patch->addSubPatch(PatchGeneric::SubPatch{ PatchGeneric::SUB_ENTRY, iBase, baseLine, modLine });

			iBase--;
			iMod--;
		}
		break;
		case ADD:
		{
			patch->addSubPatch(PatchGeneric::SubPatch{ PatchGeneric::ADD_ENTRY, iBase, baseLine, modLine });

			iMod--;
		}
		break;
		case REM:
		{
			patch->addSubPatch(PatchGeneric::SubPatch{ PatchGeneric::REM_ENTRY, iBase, baseLine, modLine });

			iBase--;
		}
		break;
		}
	}

	for (int i = 0; i <= baseFile.size(); i++)
	{
		delete[] distance[i];
	}

	for (int i = 0; i <= baseFile.size(); i++)
	{
		delete[] operation[i];
	}

	delete[] distance;
	delete[] operation;
}

std::vector<int> &PatcherGeneric::translations(const std::string &file)
{
	if (m_baseTranslations.find(file) == m_baseTranslations.end())
	{
		throw std::exception(("Translation list not initialized for " + file).c_str());
	}

	return m_baseTranslations.at(file);
}

void PatcherGeneric::updateTranslationsAfter(const std::string &file, int index, int diff)
{
	std::vector<int> &baseTranslations = translations(file);

	for (int i = index + 1; i < baseTranslations.size(); i++)
	{
		if (baseTranslations[i] != -1)
			baseTranslations[i] += diff;
	}
}

bool PatcherGeneric::getline(std::istream &stream, std::string &line) const
{
	if (m_newlineEOF)
	{
		if (util::getline(stream, line))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	line = "";

	bool bReadSomething = false;

	while (!stream.eof())
	{
		char c = stream.get();

		bReadSomething = true;

		// Otherwise we seemingly read an invalid character
		if (stream.eof())
			break;

		if (c == '\n')
			return true;
		else if (c != '\r')
		{
			line += c;
		}
	}

	if (bReadSomething)
		return true;

	return false;
}

std::vector<std::string> PatcherGeneric::loadFile(std::istream &stream) const
{
	std::vector<std::string> file;

	std::string line;

	while (getline(stream, line))
	{
		file.push_back(line);
	}

	return file;
}

void PatcherGeneric::writeFile(std::ostream &stream, const std::vector<std::string> &file) const
{
	for (int i = 0; i < (file.size() - 1); i++)
	{
		stream << file[i] << std::endl;
	}

	stream << file[file.size() - 1];

	if (m_newlineEOF)
		stream << std::endl;
}

PatcherGeneric::PatcherGeneric(const std::string &extension, bool newlineEOF) : m_extension(extension), m_newlineEOF(newlineEOF)
{

}

const char * const PatcherGeneric::extension() const
{
	return m_extension.c_str();
}

std::vector<IResourcePatch*> PatcherGeneric::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files)
{
	std::vector<IResourcePatch*> patches;

	for (const std::string &modFile : files)
	{
		std::string filename = modFile.substr(modFile.find_last_of('/') + 1);

		try
		{
			std::istream *fsBase = merger.resourcePacks().openFile(filename);

			PatchGeneric *patch = new PatchGeneric(filename, modFile, *this);

			std::ifstream fsMod(modFile);

			if (fsMod.fail())
			{
				throw std::exception(("Couldn't open file \"" + modFile + "\" for reading").c_str());
			}

			std::vector<std::string> fileBase = loadFile(*fsBase);
			std::vector<std::string> fileMod = loadFile(fsMod);

			// Initialize base translations that later will be modified
			std::vector<int> vecTranslations;

			for (int i = 0; i < fileBase.size(); i++)
			{
				vecTranslations.push_back(i);
			}

			m_baseTranslations[filename] = vecTranslations;

			buildPatch(merger, mod, patch, fileBase, fileMod);

			patches.push_back(patch);

			delete fsBase;
		}
		catch (std::exception e)
		{
			// This is a newly added file, because of that, we can't handle any conflicts and lock this file to this mod
			merger.lockFile(filename);

			patches.push_back(new PatchGeneric(filename, modFile, *this));
		}
	}

	return patches;
}

void PatcherGeneric::clearMemory()
{
	m_baseTranslations.clear();
}

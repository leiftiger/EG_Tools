#include "ModPack.h"

ModPack::ModPack(const std::string &name) : m_name(name)
{

}

const std::string &ModPack::name() const
{
	return m_name;
}

void ModPack::addFile(const std::string &file)
{
	m_files.push_back(file);
}

const std::vector<std::string> &ModPack::files() const
{
	return m_files;
}

std::istream *ModPack::openFile(const std::string &filename) const
{
	return new std::ifstream(filename);
}

int ModPack::translateDescID(int descID) const
{
	if (m_descTranslations.find(descID) != m_descTranslations.end())
		return m_descTranslations.at(descID);

	return descID;
}

int ModPack::translateDescID(int descID, ResourceMerger &merger)
{
	if (m_descTranslations.find(descID) == m_descTranslations.end() && !merger.isDescDefined(descID))
	{
		if (descID != ATN_UNDEF_VALUE || descID != ATN_UNDEF_VALUE2)
			setDescTranslation(descID, merger.reserveDescID(merger.descClass(descID)));
	}

	return translateDescID(descID);
}

void ModPack::setDescTranslation(int original, int global)
{
	m_descTranslations[original] = global;
}

int ModPack::translateUniqueID(int uniqueID) const
{
	if (m_uniqueTranslations.find(uniqueID) != m_uniqueTranslations.end())
		return m_uniqueTranslations.at(uniqueID);

	return uniqueID;
}

int ModPack::translateUniqueID(int uniqueID, ResourceMerger &merger)
{
	if (m_uniqueTranslations.find(uniqueID) == m_uniqueTranslations.end())
		setUniqueTranslation(uniqueID, merger.reserveUniqueID());

	return translateUniqueID(uniqueID);
}

void ModPack::setUniqueTranslation(int original, int global)
{
	m_uniqueTranslations[original] = global;
}

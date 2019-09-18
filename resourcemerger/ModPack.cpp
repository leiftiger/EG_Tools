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

int ModPack::translateDescID(int descID)
{
	if (m_descTranslations.find(descID) != m_descTranslations.end())
		return m_descTranslations.at(descID);

	return descID;
}

void ModPack::setDescTranslation(int original, int global)
{
	m_descTranslations[original] = global;
}

#include "RL_PackLoader.h"

RL::PackLoader::PackLoader(const ResourcePack &erbFile) : m_resourcePack(erbFile), FileLoader(".")
{

}

std::istream *RL::PackLoader::openFile(const std::string &filename) const
{
	return m_resourcePack.openFile(filename);
}

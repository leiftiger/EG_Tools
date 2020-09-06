#include "RP_PackLoader.h"

RL::PackLoader::PackLoader(const ResourcePack &erbFile) : FileLoader("."), m_resourcePack(erbFile)
{

}

std::istream *RL::PackLoader::openFile(const std::string &filename) const
{
	return m_resourcePack.openFile(filename);
}

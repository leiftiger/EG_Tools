#include "PatchCopy.h"

PatchCopy::PatchCopy(const std::string &filename, const std::string &modFilename, const ModPack &modPack) : IResourcePatch(filename, modFilename), m_modPack(modPack)
{

}

std::vector<std::string> PatchCopy::apply(std::istream &in, std::ostream &out) const
{
	return apply(m_modPack, out);
}
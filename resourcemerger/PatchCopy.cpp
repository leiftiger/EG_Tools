#include "PatchCopy.h"

PatchCopy::PatchCopy(const std::string &filename, const std::string &modFilename, const ModPack &modPack) : IResourcePatch({ filename }, { modFilename }), m_modPack(modPack)
{

}

std::vector<std::string> PatchCopy::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	return IResourcePatch::apply(m_modPack, outStreams);
}
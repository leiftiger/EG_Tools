#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

// A basic patch that copies a mod file directly without doing any delta checking
class PatchCopy : public IResourcePatch
{
protected:

	const ModPack &m_modPack;

public:

	PatchCopy(const std::string &filename, const std::string &modFilename, const ModPack &modPack);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;
};
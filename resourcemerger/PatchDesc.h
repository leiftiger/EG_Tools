#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

// A patch for an entity description file
class PatchDesc : public IResourcePatch
{
public:

	PatchDesc(const std::string &baseFilename, const std::string &modFilename);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;
};

class PatcherDesc : public IResourcePatcher
{
public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;
};
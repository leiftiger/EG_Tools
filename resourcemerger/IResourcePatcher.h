#pragma once

#include "IResourcePatch.h"
#include "ResourceMerger.h"

#include "ModPack.h"

#include <vector>

// Forward declared
class ResourceMerger; class IResourcePatch; class ModPack;

class IResourcePatcher
{
public:

	virtual ~IResourcePatcher() = default;

	// Returns the file extension that the patcher can make patches for
	virtual const char * const extension() const = 0;

	// Creates a series of patches from the given files and base resource packs
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) = 0;

	// Clears any global references held by this patcher for another merge session
	virtual void clearMemory();
};


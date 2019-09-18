#pragma once

#include "IResourcePatch.h"
#include "ResourceMerger.h"

#include <vector>

// Forward declared
class ResourceMerger;

class IResourcePatcher
{
public:

	// Returns the file extension that the patcher can make patches for
	virtual const char * const extension() const = 0;

	// Creates a series of patches from the given files and base resource packs
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, const std::vector<std::string> &files) const = 0;
};


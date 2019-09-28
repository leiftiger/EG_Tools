#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include "PatchGeneric.h"

#include "StringList.h"

// A patch for a string entry file
class PatchStrings : public IResourcePatch
{
public:

	enum TYPE : int
	{
		ADD_ENTRY,
		MOD_ENTRY,
		REM_ENTRY,
		ADD_INCLUDE,
		REM_INCLUDE
	};

	// The sub patch works per key and include entry
	struct SubPatch
	{
		TYPE m_type;

		std::pair<std::string, std::string> m_base;
		std::pair<std::string, std::string> m_mod;
	};

protected:

	std::vector<SubPatch> m_subPatches;

public:

	PatchStrings(const std::string &baseFilename, const std::string &modFilename);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;
	virtual std::vector<std::string> apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);
};

class PatcherStrings : public IResourcePatcher
{
protected:

	// If any key definition depends on a desc ID, it's updated in case it was added by a mod
	void updateDescReferences(ResourceMerger &merger, ModPack &mod, StringList &list);

	// Builds subpatches using the given definition differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchStrings *patch, const StringList &baseList, const StringList &modList) const;

public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;
};


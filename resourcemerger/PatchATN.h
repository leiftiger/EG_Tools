#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include "ATN/ATN_Manager.h"
#include "ATN/ATN_DeltaPatching.h"

// Forward declared
class PatcherATN;

// A patch for all ATN lists
class PatchATN : public IResourcePatch
{
public:

	enum TYPE : int
	{
		ADD_ENTRY,
		MOD_ENTRY,
		REM_ENTRY
	};

	// The sub patch works on an object basis, for which they may be several
	struct SubPatch
	{
		int m_file;

		TYPE m_type;

		ATN::Entry *m_modEntry;
		ATN::Entry *m_baseEntry;
	};

protected:

	std::vector<SubPatch> m_subPatches;
	PatcherATN &m_patcher;

public:

	PatchATN(PatcherATN &patcher);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;
	virtual std::vector<std::string> apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);

	const std::vector<SubPatch> &subPatches() const;
};

class PatcherATN : public IResourcePatcher
{
protected:

	std::vector<ATN::List<ATN::Entry>*> m_managedLists;

	std::vector<ATN::List<ATN::Entry>*> m_baseLists;

	// Maintain where base entries have moved for patching purposes
	std::unordered_map<int, std::unordered_map<std::int32_t, ATN::DeltaMemory>> m_baseTranslations;

	// Modifies an ATN entry so that it won't conflict with any other mod
	void modifyEntryPointers(ResourceMerger &merger, ModPack &mod, const ATN::List<ATN::Entry> &baseList, ATN::Entry *entry, bool changeIDs) const;

	// Constructs a patch for the given lists
	PatchATN *buildPatch(ResourceMerger &merger, ModPack &mod, int listNum, ATN::List<ATN::Entry> &baseList, ATN::List<ATN::Entry> &modList);

	// Gets the translations for a specific file
	ATN::DeltaMemory &translations(int file, std::int32_t uniqueID);

	friend class PatchATN;

public:

	~PatcherATN();

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;

	virtual void clearMemory() override;
};
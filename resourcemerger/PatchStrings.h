#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include "PatchGeneric.h"

class StringList
{
protected:

	std::vector<std::string> m_includes;

	std::vector<std::string> m_keys;
	std::unordered_map<std::string, std::string> m_map;

public:

	// Adds an include file to the list
	void addInclude(const std::string &include);

	// Removes an include file to the list
	void removeInclude(const std::string &include);

	// Returns the include files in this list
	const std::vector<std::string> &includes() const;

	// Adds a new key to the list, or sets it if it already exists
	void set(const std::string &key, const std::string &value);

	// Gets the value of the specified key
	const std::string &get(const std::string &key) const;

	// Returns whether or not this list has the specified key
	bool contains(const std::string &key) const;

	// Removes the key from the list
	void remove(const std::string &key);

	// Returns the keys of this list in write order
	const std::vector<std::string> &keys() const;

	friend std::istream &operator>>(std::istream &stream, StringList &list);
	friend std::ostream &operator<<(std::ostream &stream, const StringList &list);
};

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

	// Returns true if the string only contains numeric characters
	bool isNumeric(const std::string &str) const;

	// If any key definition depends on a desc ID, it's updated in case it was added by a mod
	void updateDescReferences(ResourceMerger &merger, ModPack &mod, StringList &list);

	// Builds subpatches using the given definition differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchStrings *patch, const StringList &baseList, const StringList &modList) const;

public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;
};


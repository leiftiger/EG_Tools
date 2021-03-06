#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class EntityDesc
{
protected:

	std::vector<std::string> m_keys;
	std::vector<std::string> m_values;

	bool m_newlineEOF = false;

	// Reads a line from the stream, returning true if a line was read, or false if the stream is at EOF
	bool getline(std::istream &stream, std::string &line);

public:

	// Number of elements in this entity description
	std::size_t size() const;

	void clear();

	void insert(std::size_t pos, const std::string &key, const std::string &value);

	// Adds the key and value last in the list
	void push_back(const std::string &key, const std::string &value);

	void set(std::size_t pos, const std::string &key, const std::string &value);

	void remove(std::size_t pos);
	void remove(const std::string &key, const std::string &value);

	const std::vector<std::string> &keys() const;
	const std::vector<std::string> &values() const;

	friend std::istream &operator>>(std::istream &stream, EntityDesc &desc);
	friend std::ostream &operator<<(std::ostream &stream, const EntityDesc &desc);
};



// Forward declared
class PatcherDesc;

// A patch for an entity description file
class PatchDesc : public IResourcePatch
{
public:

	enum TYPE : int
	{
		ADD_ENTRY,
		MOD_ENTRY,
		REM_ENTRY,
		SUB_ENTRY
	};

	// The sub patch works on a group basis per file
	struct SubPatch
	{
		TYPE m_type;

		// Base index add / remove / modify
		int m_pos;

		std::pair<std::string, std::string> m_base;
		std::pair<std::string, std::string> m_mod;
	};

protected:

	std::vector<SubPatch> m_subPatches;
	PatcherDesc &m_patcher;

public:

	PatchDesc(const std::string &baseFilename, const std::string &modFilename, PatcherDesc &patcher);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;
	virtual std::vector<std::string> apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);

	// Update all desc ID references to the ones defined in the mod pack
	static void updateDescIDs(EntityDesc &desc, const ModPack &mod);
	// Update all desc ID references to the ones defined in the mod pack and reserved by the resource merger
	static void updateDescIDs(EntityDesc &desc, ModPack &mod, ResourceMerger &merger);
};

class PatcherDesc : public IResourcePatcher
{
protected:

	// Maintain where base entries have moved for patching purposes
	std::unordered_map<std::string, std::vector<int>> m_baseTranslations;

	// Builds subpatches using the given description differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchDesc *patch, const EntityDesc &baseDesc, const EntityDesc &modDesc) const;

	// Gets the translations for a specific file
	std::vector<int> &translations(const std::string &file);

	// Updates all translations after index by diff to counter a new or removed translation
	void updateTranslationsAfter(const std::string &file, int index, int diff);

	friend class PatchDesc;

public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;
	
	virtual void clearMemory() override;
};

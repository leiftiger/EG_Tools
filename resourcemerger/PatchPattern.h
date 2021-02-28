#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class EntityPattern
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

	friend std::istream &operator>>(std::istream &stream, EntityPattern &desc);
	friend std::ostream &operator<<(std::ostream &stream, const EntityPattern &desc);
};



// Forward declared
class PatcherPattern;

// A patch for a pattern file
class PatchPattern : public IResourcePatch
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
	PatcherPattern &m_patcher;

public:

	PatchPattern(const std::string &baseFilename, const std::string &modFilename, PatcherPattern &patcher);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;
	virtual std::vector<std::string> apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);
};

class PatcherPattern : public IResourcePatcher
{
protected:

	// Maintain where base entries have moved for patching purposes
	std::unordered_map<std::string, std::vector<int>> m_baseTranslations;

	// Builds subpatches using the given description differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchPattern *patch, const EntityPattern &baseDesc, const EntityPattern &modDesc) const;

	// Gets the translations for a specific file
	std::vector<int> &translations(const std::string &file);

	// Updates all translations after index by diff to counter a new or removed translation
	void updateTranslationsAfter(const std::string &file, int index, int diff);

	friend class PatchPattern;

public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;

	virtual void clearMemory() override;
};

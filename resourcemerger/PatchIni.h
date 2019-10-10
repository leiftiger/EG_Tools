#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include "PatchGeneric.h"

class IniList
{
protected:

	std::vector<std::string> m_keys;
	std::vector<std::string> m_values;

	char m_keyDelimiter;

	bool m_newlineEOF = false;

	// Reads a line from the stream, returning true if a line was read, or false if the stream is at EOF
	bool getline(std::istream &stream, std::string &line);

public:

	IniList(char keyDelimiter);

	// Number of elements in this list
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

	friend std::istream &operator>>(std::istream &stream, IniList &list);
	friend std::ostream &operator<<(std::ostream &stream, const IniList &list);
};

// Forward declared
class PatcherIni;

// A patch for certain ini files
class PatchIni : public IResourcePatch
{
public:

	enum TYPE : int
	{
		ADD_ENTRY,
		MOD_ENTRY,
		REM_ENTRY,
		SUB_ENTRY
	};

	// The sub patch works per key and include entry
	struct SubPatch
	{
		TYPE m_type;

		int m_pos;

		std::pair<std::string, std::string> m_base;
		std::pair<std::string, std::string> m_mod;
	};

protected:

	std::vector<SubPatch> m_subPatches;

	PatcherIni &m_patcher;

	char m_keyDelimiter;

public:

	PatchIni(const std::string &baseFilename, const std::string &modFilename, PatcherIni &patcher, char keyDelimiter);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);
};

class PatcherIni : public IResourcePatcher
{
protected:

	// Maintain where base entries have moved for patching purposes
	std::unordered_map<std::string, std::vector<int>> m_baseTranslations;

	// Builds subpatches using the given list differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchIni *patch, const IniList &baseList, const IniList &modList) const;

	// Gets the translations for a specific file
	std::vector<int> &translations(const std::string &file);

	// Updates all translations after index by diff to counter a new or removed translation
	void updateTranslationsAfter(const std::string &file, int index, int diff);

	friend class PatchIni;

public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;

	virtual void clearMemory() override;
};


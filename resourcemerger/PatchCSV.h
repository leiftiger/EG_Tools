#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class SpreadsheetCSV
{
protected:

	std::vector<std::string> m_headers;

	std::vector<std::vector<std::string>> m_rows;

	// Primary keys that are defined in this spreadsheet
	std::unordered_set<std::string> m_keys;

public:

	// Number of rows in this CSV spreadsheet
	std::size_t size() const;

	void clear();

	void insertRow(std::size_t pos, std::vector<std::string> row);

	// Adds the row last in thelist
	void addRow(std::vector<std::string> row);

	void removeRow(std::size_t pos);

	// Returns whether or not the list has the given key in the first column of the CSV
	bool hasKey(const std::string &key) const;

	// Change the given primary key due to external changes
	void changeKey(const std::string &original, const std::string &change);

	const std::vector<std::string> &headers() const;

	std::vector<std::string> &row(std::size_t pos);
	const std::vector<std::string> &row(std::size_t pos) const;

	static std::vector<std::string> splitString(std::string str, char delimiter);
	static std::string concatString(const std::vector<std::string> &strList, char delimiter);

	friend std::istream &operator>>(std::istream &stream, SpreadsheetCSV &list);
	friend std::ostream &operator<<(std::ostream &stream, const SpreadsheetCSV &list);
};

// Forward declared
class PatcherCSV;

// A patch for comma-separated lists
class PatchCSV : public IResourcePatch
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

		std::vector<std::string> m_base;
		std::vector<std::string> m_mod;
	};

protected:

	std::vector<SubPatch> m_subPatches;
	PatcherCSV &m_patcher;

public:

	PatchCSV(const std::string &baseFilename, const std::string &modFilename, PatcherCSV &patcher);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);
};

class PatcherCSV : public IResourcePatcher
{
protected:

	// Maintain where base entries have moved for patching purposes
	std::unordered_map<std::string, std::vector<int>> m_baseTranslations;

	// Builds subpatches using the given list differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchCSV *patch, const SpreadsheetCSV &baseList, const SpreadsheetCSV &modList) const;

	// Gets the translations for a specific file
	std::vector<int> &translations(const std::string &file);

	// Updates all translations after index by diff to counter a new or removed translation
	void updateTranslationsAfter(const std::string &file, int index, int diff);

	// If any key definition depends on a desc ID, it's updated in case it was added by a mod
	void updateDescReferences(ResourceMerger &merger, ModPack &mod, SpreadsheetCSV &list);

	friend class PatchCSV;

public:

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;

	virtual void clearMemory() override;
};

#pragma once

#include "IResourcePatch.h"
#include "IResourcePatcher.h"

// Forward declared
class PatcherGeneric;

// A generic patcher for text-based files
class PatchGeneric : public IResourcePatch
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

		std::string m_base;
		std::string m_mod;
	};

protected:

	std::vector<SubPatch> m_subPatches;
	PatcherGeneric &m_patcher;

	void insert(std::size_t pos, const std::string &line, std::vector<std::string> &file) const;
	void remove(std::size_t pos, std::vector<std::string> &file) const;

public:

	PatchGeneric(const std::string &baseFilename, const std::string &modFilename, PatcherGeneric &patcher);

	virtual std::vector<std::string> apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const override;

	void addSubPatch(SubPatch patch);
};

class PatcherGeneric : public IResourcePatcher
{
protected:

	// Maintain where base entries have moved for patching purposes
	std::unordered_map<std::string, std::vector<int>> m_baseTranslations;

	std::string m_extension;

	// End every line with a newline or allow EOF in last line
	bool m_newlineEOF = false;

	// Builds subpatches using the given file differences
	void buildPatch(ResourceMerger &merger, ModPack &mod, PatchGeneric *patch, const std::vector<std::string> &baseFile, const std::vector<std::string> &modFile) const;

	// Gets the translations for a specific file
	std::vector<int> &translations(const std::string &file);

	// Updates all translations after index by diff to counter a new or removed translation
	void updateTranslationsAfter(const std::string &file, int index, int diff);

	// Reads a line from the given stream, returns false if the stream is already at EOF
	bool getline(std::istream &stream, std::string &line) const;

	std::vector<std::string> loadFile(std::istream &stream) const;
	void writeFile(std::ostream &stream, const std::vector<std::string> &file) const;

	friend class PatchGeneric;

public:

	PatcherGeneric(const std::string &extension, bool newlineEOF);

	virtual const char * const extension() const override;
	virtual std::vector<IResourcePatch*> createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &files) override;
};
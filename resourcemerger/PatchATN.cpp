#include "PatchATN.h"

#include "utils.h"

// Can ignore modFilenames here since apply(ModPack, outStreams) should never be called for this
// Because the ATN files can share definitions, we must always load all of them to prevent errors during patching
PatchATN::PatchATN(PatcherATN &patcher) : IResourcePatch({ "ATNData.ros", "ATNData_Objects.ros", "ATNData_Tutorials.ros", "ATNResources.ros" }, {}), m_patcher(patcher)
{

}

std::vector<std::string> PatchATN::apply(std::vector<std::istream*> &inStreams, std::vector<std::ostream*> &outStreams) const
{
	std::vector<std::string> output;

	std::vector<ATN::List<ATN::Entry>*> lists;

	// First pass
	for (std::size_t i = 0; i < inStreams.size(); i++)
	{
		ATN::List<ATN::Entry> *list = new ATN::List<ATN::Entry>("ATN File " + std::to_string(i));

		util::parseATN(*inStreams[i], *list, false);

		lists.push_back(list);

		ATN::Manager::addList(list);
	}

	// Second pass
	for (std::size_t i = 0; i < inStreams.size(); i++)
	{
		inStreams[i]->seekg(0, std::ios::beg);

		util::parseATN(*inStreams[i], *lists[i], true);
	}

	for (const PatchATN::SubPatch &patch : m_subPatches)
	{
		ATN::List<ATN::Entry>* list = lists[patch.m_file];

		switch (patch.m_type)
		{
		case PatchATN::ADD_ENTRY:
			list->add(*patch.m_modEntry);
			break;
		case PatchATN::MOD_ENTRY:
		{
			if (list->contains(patch.m_modEntry->id()))
			{
				ATN::Entry &curEntry = list->find(patch.m_modEntry->id());

				curEntry.applyChanges(*patch.m_baseEntry, *patch.m_modEntry, m_patcher.translations(patch.m_file, patch.m_modEntry->id()));
			}
			else
			{
				output.push_back("Couldn't find ATN \"" + patch.m_modEntry->name() + "\" (" + std::to_string(patch.m_modEntry->id()) + "), possibly removed");
			}
		}
			break;
		case PatchATN::REM_ENTRY:
			list->remove(*patch.m_baseEntry);
			break;
		}
	}

	// Write back to files
	for (std::size_t i = 0; i < outStreams.size(); i++)
	{
		ATN::List<ATN::Entry> *list = lists[i];

		util::writeATN(*outStreams[i], *list);
	}

	// Clear manager references to a clean state and clear list memories
	ATN::Manager::clear();

	output.push_back("Patched ATNs");

	return output;
}

std::vector<std::string> PatchATN::apply(const ModPack &mod, std::vector<std::ostream*> &outStreams) const
{
	return std::vector<std::string>({"An ATN file wasn't found in base game, is a resource pack not loaded?"});
}

void PatchATN::addSubPatch(SubPatch patch)
{
	m_subPatches.push_back(patch);
}

const std::vector<PatchATN::SubPatch> &PatchATN::subPatches() const
{
	return m_subPatches;
}

void PatcherATN::modifyEntryPointers(ResourceMerger &merger, ModPack &mod, const ATN::List<ATN::Entry> &baseList, ATN::Entry *entry, bool changeIDs = true) const
{
	// As effects and percepts depend on the executable to work, we won't perform any modification on those entries
	// they may just be a duplicate from another ATN list, which is prevalent in the base files
	if (changeIDs && typeid(*entry) != typeid(ATN::Effect) && typeid(*entry) != typeid(ATN::Percept))
	{
		// Translate its unique ID
		entry->setID(mod.translateUniqueID(entry->id(), merger));
	}

	if (typeid(*entry) == typeid(ATN::Network))
	{
		ATN::Network *net = (ATN::Network*)entry;

		// Just in case this network is launched manually, the default values are updated as well
		for (std::size_t i = 0; i < net->parameters().size(); i++)
		{
			ATN::Parameter *param = net->parameters()[i];

			if (param->m_type == "Act Of Infamy ID" || param->m_type == "Agent Group Type" || param->m_type == "Entity Type" || param->m_type == "Room Type")
			{
				param->m_defaultValue = mod.translateDescID((int)param->m_defaultValue, merger);
			}

			// Also update network IDs, unless it's a base game ID
			if (param->m_type == "Network ID" && !baseList.contains((std::uint32_t)param->m_defaultValue))
			{
				param->m_defaultValue = mod.translateUniqueID((int)param->m_defaultValue, merger);
			}
		}
	}
	else if (typeid(*entry) == typeid(ATN::State))
	{
		ATN::State *state = (ATN::State*)entry;

		if (state->networkTransition() != nullptr)
		{
			ATN::Network *net = state->networkTransition();

			// Since TATNEffectCreateAgentGroup takes an int instead of desc ID, we have to manually make sure that those references also get updated desc IDs
			if (net->name() == "CharacterMissionCreate" && state->parameterMarshalls()[0]->m_type != ATN::ParameterMarshallType::ParameterIndex)
				state->parameterMarshalls()[0]->m_value = mod.translateDescID((int)state->parameterMarshalls()[0]->m_value, merger);
			else if (net->name() == "Create An Agent" && state->parameterMarshalls()[2]->m_type != ATN::ParameterMarshallType::ParameterIndex)
				state->parameterMarshalls()[2]->m_value = mod.translateDescID((int)state->parameterMarshalls()[2]->m_value, merger);

			// For normal cases, it is however enough to just match the proper type
			for (std::size_t i = 0; i < net->parameters().size(); i++)
			{
				ATN::Parameter *param = net->parameters()[i];
				ATN::ParameterMarshall *marshall = state->parameterMarshalls()[i];

				if (marshall->m_type != ATN::ParameterMarshallType::ParameterIndex && (param->m_type == "Act Of Infamy ID" || param->m_type == "Agent Group Type" || param->m_type == "Entity Type" || param->m_type == "Room Type"))
				{
					marshall->m_value = mod.translateDescID((int)marshall->m_value, merger);
				}

				// Also update network IDs, unless it's a base game ID
				if (marshall->m_type != ATN::ParameterMarshallType::ParameterIndex && param->m_type == "Network ID" && !baseList.contains((std::uint32_t)marshall->m_value))
				{
					marshall->m_value = mod.translateUniqueID((int)marshall->m_value, merger);
				}
			}
		}
	}
	else if (typeid(*entry) == typeid(ATN::Transition))
	{
		ATN::Transition *transition = (ATN::Transition*)entry;

		if (transition->effect() != nullptr)
		{
			ATN::Effect *effect = transition->effect();

			// As noted above, this effect uses int for desc IDs rather than proper types, which forces us to check for this specifically
			if (effect->gameFunction() == "TATNEffectCreateAgentGroup")
			{
				// Agent desc ID
				if (transition->effectParameterMarshalls()[2]->m_type != ATN::ParameterMarshallType::ParameterIndex)
					transition->effectParameterMarshalls()[2]->m_value = mod.translateDescID((int)transition->effectParameterMarshalls()[2]->m_value, merger);
				// Interested object desc ID
				if (transition->effectParameterMarshalls()[4]->m_type != ATN::ParameterMarshallType::ParameterIndex)
					transition->effectParameterMarshalls()[4]->m_value = mod.translateDescID((int)transition->effectParameterMarshalls()[4]->m_value, merger);
			}

			// For other effects, and this one, check also for proper types
			for (std::size_t i = 0; i < effect->parameters().size(); i++)
			{
				ATN::Parameter *param = effect->parameters()[i];
				ATN::ParameterMarshall *marshall = transition->effectParameterMarshalls()[i];

				if (marshall->m_type != ATN::ParameterMarshallType::ParameterIndex && (param->m_type == "Act Of Infamy ID" || param->m_type == "Agent Group Type" || param->m_type == "Entity Type" || param->m_type == "Room Type"))
				{
					marshall->m_value = mod.translateDescID((int)marshall->m_value, merger);
				}

				// Also update network IDs, unless it's a base game ID
				if (marshall->m_type != ATN::ParameterMarshallType::ParameterIndex && param->m_type == "Network ID" && !baseList.contains((std::uint32_t)marshall->m_value))
				{
					marshall->m_value = mod.translateUniqueID((int)marshall->m_value, merger);
				}
			}
		}

		ATN::Percept *percept = transition->percept();

		// Do the same for percepts, which seemingly only uses proper types, so no need for special handling
		for (std::size_t i = 0; i < percept->parameters().size(); i++)
		{
			ATN::Parameter *param = percept->parameters()[i];
			ATN::ParameterMarshall *marshall = transition->perceptParameterMarshalls()[i];

			if (marshall->m_type != ATN::ParameterMarshallType::ParameterIndex && (param->m_type == "Act Of Infamy ID" || param->m_type == "Agent Group Type" || param->m_type == "Entity Type" || param->m_type == "Room Type"))
			{
				marshall->m_value = mod.translateDescID((int)marshall->m_value, merger);
			}

			// Also update network IDs, unless it's a base game ID
			if (marshall->m_type != ATN::ParameterMarshallType::ParameterIndex && param->m_type == "Network ID" && !baseList.contains((std::uint32_t)marshall->m_value))
			{
				marshall->m_value = mod.translateUniqueID((int)marshall->m_value, merger);
			}
		}
	}
}

PatchATN *PatcherATN::buildPatch(ResourceMerger &merger, ModPack &mod, int listNum, ATN::List<ATN::Entry> &baseList, ATN::List<ATN::Entry> &modList)
{
	PatchATN* patch = new PatchATN(*this);

	for (std::pair<const std::uint32_t, ATN::IATN_Data*> &pair : modList)
	{
		ATN::Entry *modEntry = (ATN::Entry*)pair.second;

		if (!baseList.contains(modEntry->id()))
		{
			// New addition, can add entire entry without issues
			modifyEntryPointers(merger, mod, baseList, modEntry, true);

			patch->addSubPatch(PatchATN::SubPatch{ listNum, PatchATN::ADD_ENTRY, modEntry, nullptr });
		}
		else
		{
			ATN::Entry *baseEntry = &baseList.find(modEntry->id());

			// Modified entry, need to check properties
			if (*modEntry != *baseEntry)
			{
				// We only modify desc ID pointers since the unique ID was already present in the base game
				modifyEntryPointers(merger, mod, baseList, modEntry, false);

				if (typeid(*modEntry) != typeid(*baseEntry))
					throw ATN::Exception("Illegal mod entry, modified type of existing ID (%s != %s)", modEntry->typeName(),  baseEntry->typeName());
				else
				{
					patch->addSubPatch(PatchATN::SubPatch{ listNum, PatchATN::MOD_ENTRY, modEntry, baseEntry });
				}
			}
		}
	}

	for (std::pair<const std::uint32_t, ATN::IATN_Data*> &pair : baseList)
	{
		ATN::Entry *baseEntry = (ATN::Entry*)pair.second;

		if (!modList.contains(baseEntry->id()))
		{
			// We assume here that the ATN Editor has done all of the work for us, so there should be no dangling references from doing this
			// Moreover, it should hopefully be very rare for it to be necessary to erase game logic, as this will cause all sorts of issues, least of all with mod dependencies
			patch->addSubPatch(PatchATN::SubPatch{ listNum, PatchATN::REM_ENTRY, nullptr, baseEntry });
		}
	}

	return patch;
}

ATN::DeltaMemory &PatcherATN::translations(int file, std::int32_t uniqueID)
{
	if (m_baseTranslations.find(file) == m_baseTranslations.end())
	{
		m_baseTranslations[file] = std::unordered_map<std::int32_t, ATN::DeltaMemory>();
	}

	std::unordered_map<std::int32_t, ATN::DeltaMemory> &allTranslations = m_baseTranslations[file];

	if (allTranslations.find(uniqueID) == allTranslations.end())
		allTranslations[uniqueID] = ATN::DeltaMemory();

	return allTranslations[uniqueID];
}

PatcherATN::~PatcherATN()
{
	for (ATN::List<ATN::Entry> *list : m_managedLists)
	{
		delete list;
	}

	for (ATN::List<ATN::Entry> *list : m_baseLists)
	{
		delete list;
	}
}

const char * const PatcherATN::extension() const
{
	return ".ros";
}

std::vector<IResourcePatch*> PatcherATN::createPatches(ResourceMerger &merger, ModPack &mod, const std::vector<std::string> &modFiles)
{
	std::vector<std::string> baseFiles({ "ATNData.ros", "ATNData_Objects.ros", "ATNData_Tutorials.ros", "ATNResources.ros" });

	std::vector<ATN::List<ATN::Entry>*> baseLists;
	std::vector<ATN::List<ATN::Entry>*> modLists;

	// We only load the base lists once as they won't change between any mod
	if (m_baseLists.empty())
	{
		// First pass
		for (std::string &baseFile : baseFiles)
		{
			std::istream *fs = merger.resourcePacks().openFile(baseFile);

			ATN::List<ATN::Entry> *list = new ATN::List<ATN::Entry>("ERB: " + baseFile);

			util::parseATN(*fs, *list, false);

			baseLists.push_back(list);

			ATN::Manager::addList(list);

			delete fs;
		}

		// Second pass
		for (std::size_t i = 0; i < baseFiles.size(); i++)
		{
			std::string &baseFile = baseFiles[i];

			std::istream *fs = merger.resourcePacks().openFile(baseFile);

			util::parseATN(*fs, *baseLists[i], true);

			delete fs;
		}

		for (ATN::List<ATN::Entry> *list : baseLists)
		{
			ATN::Manager::removeList(list);
		}

		// Clear manager from references for next load
		ATN::Manager::clear();

		m_baseLists = baseLists;
	}
	else
	{
		baseLists = m_baseLists;
	}

	// First pass
	for (std::size_t i = 0; i < baseFiles.size(); i++)
	{
		std::string &baseFile = baseFiles[i];

		bool wasModded = false;

		for (const std::string &modFile : modFiles)
		{
			if (modFile.substr(modFile.length() - baseFile.length()) != baseFile)
				continue;

			wasModded = true;

			std::ifstream fs(modFile);

			ATN::List<ATN::Entry> *list = new ATN::List<ATN::Entry>("Mod: " + modFile);

			util::parseATN(fs, *list, false);

			modLists.push_back(list);

			ATN::Manager::addList(list);

			break;
		}

		// If not modded, load base file for references
		if (!wasModded)
			ATN::Manager::addList(baseLists[i]);
	}

	// Second pass
	for (std::size_t i = 0; i < modFiles.size(); i++)
	{
		const std::string &modFile = modFiles[i];

		std::ifstream fs(modFile);

		util::parseATN(fs, *modLists[i], true);
	}

	// Because we load both modded and base lists, we don't know the exact combination loaded
	// So we copy the manager's references and then remove them one by one
	std::vector<ATN::List<ATN::Entry>*> loadedLists = ATN::Manager::lists();

	for (ATN::List<ATN::Entry> *list : loadedLists)
	{
		ATN::Manager::removeList(list);
	}

	// Clear manager references to a clean state
	ATN::Manager::clear();

	PatchATN *patchAdds = new PatchATN(*this);
	PatchATN *patchMods = new PatchATN(*this);
	PatchATN *patchRems = new PatchATN(*this);

	// Build patches by comparing each base file with its modded counterpart
	for (std::size_t i = 0; i < baseFiles.size(); i++)
	{
		const std::string &baseFile = baseFiles[i];

		for (std::size_t i2 = 0; i2 < modFiles.size(); i2++)
		{
			const std::string &modFile = modFiles[i2];

			if (modFile.substr(modFile.length() - baseFile.length()) != baseFile)
				continue;

			PatchATN* subPatch = buildPatch(merger, mod, i, *baseLists[i], *modLists[i2]);

			for (const PatchATN::SubPatch &patch : subPatch->subPatches())
			{
				switch (patch.m_type)
				{
				case PatchATN::ADD_ENTRY:
					patchAdds->addSubPatch(patch);
					break;
				case PatchATN::MOD_ENTRY:
					patchMods->addSubPatch(patch);
					break;
				case PatchATN::REM_ENTRY:
					patchRems->addSubPatch(patch);
					break;
				}
			}

			delete subPatch;
		}
	}

	PatchATN *masterPatch = new PatchATN(*this);

	// As some modifications may rely on objects being present, we always add objects first
	// then modify them, and then delete them, when it should be safe to do so
	for (const PatchATN::SubPatch &patch : patchAdds->subPatches())
	{
		masterPatch->addSubPatch(patch);
	}

	for (const PatchATN::SubPatch &patch : patchMods->subPatches())
	{
		masterPatch->addSubPatch(patch);
	}

	for (const PatchATN::SubPatch &patch : patchRems->subPatches())
	{
		masterPatch->addSubPatch(patch);
	}

	delete patchAdds;
	delete patchMods;
	delete patchRems;


	for (ATN::List<ATN::Entry> *list : baseLists)
	{
		m_managedLists.push_back(list);
	}

	// Finished processing everything, make sure we keep references to the memory
	// as they still depend on pointer references when patching
	for (ATN::List<ATN::Entry> *list : modLists)
	{
		m_managedLists.push_back(list);
	}

	return std::vector<IResourcePatch*>({ masterPatch });
}

void PatcherATN::clearMemory()
{
	m_baseTranslations.clear();
}

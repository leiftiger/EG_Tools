#pragma once

#include <unordered_map>
#include <vector>

#include "ATN_Effect.h"
#include "ATN_List.h"
#include "ATN_Network.h"
#include "ATN_Percept.h"
#include "ATN_Property.h"

namespace ATN
{
	// Forward-declared
	class Effect;
	class Percept;
	class Network;
	
	// Forward-declared, defined in IATN_Data
	bool compareLessThanPointersIATN(const IATN_Data *a, const IATN_Data *b);
	bool compareEqualPointersIATN(const IATN_Data *a, const IATN_Data *b);

	// Singleton manager that keeps a record of all open ATN files and such can provide references to every object, regardless of where they're stored
	class Manager
	{
	private:
		Manager();

		~Manager();

		static Manager &instance()
		{
			static Manager m;

			return m;
		}
	public:

		// Ensure the singleton cannot be copied
		Manager(Manager const &other) = delete;
		void operator=(Manager const &other) = delete;

	private:
		std::vector<List<Entry>*> m_lists;

		std::vector<Entry*> m_orphanEntries;

		std::unordered_map<std::string, List<Property>> m_descValues;

		std::unordered_map<std::string, std::string> m_interpretationFormats;

		std::vector<Effect*> m_effects;
		std::vector<Percept*> m_percepts;
		std::vector<Network*> m_networks;

		const ATN::Entry *m_storedEntry;

	public:

		// Adds a format interpretation for the specified type name
		static void addInterpretation(const std::string &type, const std::string &format);

		// Returns whether or not an interpretation exists for this type
		static bool hasInterpretation(const std::string &type);

		// Gets the interpretation format for this type
		static const std::string &getInterpretation(const std::string &type);

		// Loads a number of ATN lists from the specified file names
		static void loadFromFiles(const std::vector<std::string> &filenames);

		// Saves a number of ATN lists to file names of each list in the manager
		static void saveToFiles();

		// Adds a new list to the manager
		static void addList(List<Entry> *list);

		// Add a new entry to the global list that doesn't belong to any managed lists
		static void addOrphanEntry(Entry &el);

		// Add new entry to the global list as it has been added elsewhere
		static void addEntry(Entry &el);

		// Remove entry from global list (should only get called from ATN::List)
		static void removeEntry(const Entry &el);

		// Removes a list from the manager (note: does not clear global references)
		static void removeList(List<Entry> *list);

		// Clear all lists managed by the manager and reset all references
		static void clear();

		// Returns the maximum ID noted in the global list
		static uint32_t maxID();

		// Get all lists stored in this manager
		static const std::vector<List<Entry>*> lists();

		// Set hash definitions
		static void setDefinitions(const std::string &strType, const List<Property> &list);

		// Get hash definitions
		static List<Property> &getDefinitions(const std::string &strType);

		// Do we have hash definitions for this type?
		static bool hasDefinitions(const std::string &strType);

		// Finds an ATN entry by ID
		static Entry &findByID(std::uint32_t id);

		// Finds an ATN entry by ID and return the ATN list it belongs to
		// Slightly slower due to searching multiple ATN files
		static Entry &findByID(std::uint32_t id, List<Entry> *&outList);

		// Finds an ATN entry by name
		static Entry &findByName(const std::string &name);

		// Finds an ATN entry by ID and return the ATN list it belongs to
		// Slightly slower due to searching multiple ATN files
		static Entry &findByName(const std::string &name, List<Entry> *&outList);

		// Retrieves a list of entries that contain the specified name as well as the ATN list they belong to
		static std::vector<std::pair<Entry*, List<Entry>*>> searchName(const std::string &namePart);

		// Is this entry defined in multiple ATN lists?
		static bool isMultiDefined(const ATN::Entry &el);

		// Sets the entry's name to the specified one and updates all affected lists
		static void updateName(Entry &entry, const std::string &newName);

		// Returns all effects currently loaded
		static const std::vector<Effect*> &getEffects();

		// Returns all percepts currently loaded
		static const std::vector<Percept*> &getPercepts();

		// Returns all networks currently loaded
		static const std::vector<Network*> &getNetworks();

		// TODO: Move these to undo/redo-manager in the future

		// Get global pointer to a specific ATN Entry (used for copy/paste)
		static const ATN::Entry *getStoredEntry();
		// Set global pointer to a specific ATN Entry (used for copy/paste)
		static void setStoredEntry(const ATN::Entry *entry);
	};
}
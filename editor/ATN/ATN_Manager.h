#pragma once

#include <unordered_map>
#include <vector>

#include "ATN_List.h"
#include "ATN_Property.h"

namespace ATN
{
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

		std::unordered_map<std::string, List<Property>> m_descValues;

	public:

		static void loadFromFiles(const std::vector<std::string> &filenames);

		static void addList(List<Entry> *list);

		// Remove entry from global list (should only get called from ATN::List)
		static void removeEntry(const Entry &el);

		// Returns the maximum ID noted in the global list
		static uint32_t maxID();

		// Get all lists stored in this manager
		static const std::vector<List<Entry>*> lists();

		// Set hash definitions
		static void setDefinitions(const std::string &strType, List<Property> &list);

		// Get hash definitions
		static const List<Property> &Manager::getDefinitions(const std::string &strType);

		// Do we have hash definitions for this type?
		static bool Manager::hasDefinitions(const std::string &strType);

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
	};
}
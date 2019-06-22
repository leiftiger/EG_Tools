#pragma once

#include <vector>

#include "ATN_List.h"
#include "ATN_Property.h"

namespace ATN
{
	// Singleton manager that keeps a record of all open ATN files and such can provide references to every object, regardless of where they're stored
	class Manager
	{
	private:
		Manager()
		{
			m_lists.push_back(new List<Entry>(":Global:"));
		}

		~Manager()
		{
			// As we hold all state references in mind, we will clean up any references left in memory when the application terminates

			delete m_lists[0];

			for (size_t i = 1; i < m_lists.size(); i++)
			{
				m_lists[i]->clear();

				delete m_lists[i];
			}
		}

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

		List<Property> m_descValues;

	public:

		static void addList(List<Entry> *list);

		// Remove entry from global list (should only get called from ATN::List)
		static void removeEntry(const Entry &el);

		// Returns the maximum ID noted in the global list
		static uint32_t maxID();

		// Get all lists stored in this manager
		static const std::vector<List<Entry>*> lists();

		// Set hash definitions
		static void setHashValues(List<Property> &list);

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
	};
}
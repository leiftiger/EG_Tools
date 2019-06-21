#pragma once

#include <vector>

#include "ATN_List.h"

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

			for (int i = 1; i < m_lists.size(); i++)
			{
				m_lists[i]->clear();

				delete m_lists[i];
			}
		}
	public:

		static Manager &instance()
		{
			static Manager m;

			return m;
		}

		// Ensure the singleton cannot be copied
		Manager(Manager const &other) = delete;
		void operator=(Manager const &other) = delete;

	private:
		std::vector<List<Entry>*> m_lists;

		List<Property> m_descAnims;
		List<Property> m_descEvents;

	public:

		void addList(List<Entry> *list);

		// Remove entry from global list (should only get called from ATN::List)
		void removeEntry(const Entry &el);

		// Get all lists stored in this manager
		const std::vector<List<Entry>*> lists() const;

		// Set animation hash definitions
		void setAnims(List<Property> &list);

		// Set event hash definitions
		void setEvents(List<Property> &events);

		// Finds an ATN entry by ID
		Entry &findByID(std::uint32_t id) const;

		// Finds an ATN entry by ID and return the ATN list it belongs to
		// Slightly slower due to searching multiple ATN files
		Entry &findByID(std::uint32_t id, List<Entry> *&outList) const;

		// Finds an ATN entry by name
		Entry &findByName(const std::string &name) const;

		// Finds an ATN entry by ID and return the ATN list it belongs to
		// Slightly slower due to searching multiple ATN files
		Entry &findByName(const std::string &name, List<Entry> *&outList) const;
	};
}
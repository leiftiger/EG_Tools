#include "ATN_Manager.h"

namespace ATN
{
	void Manager::addList(List<Entry> *list)
	{
		// Copy all elements in this list to the global ATN list
		for (const std::pair<std::uint32_t, IATN_Data*> &pair : *list)
		{
			instance().m_lists[0]->add(*(Entry*)pair.second);
		}

		instance().m_lists.push_back(list);
	}
	void Manager::removeEntry(const Entry &el)
	{
		instance().m_lists[0]->remove(el);
	}

	uint32_t Manager::maxID()
	{
		return instance().m_lists[0]->maxID();
	}

	const std::vector<List<Entry>*> Manager::lists()
	{
		return instance().m_lists;
	}

	void Manager::setHashValues(List<Property> &list)
	{
		instance().m_descValues = list;
	}


	Entry &Manager::findByID(std::uint32_t id)
	{
		return instance().m_lists[0]->find(id);
	}

	Entry &Manager::findByID(std::uint32_t id, List<Entry> *&outList)
	{
		for (size_t i = 1; i < instance().m_lists.size(); i++)
		{
			try
			{
				Entry &el = instance().m_lists[i]->find(id);

				outList = instance().m_lists[i];

				return el;
			}
			catch (Exception e) {}
		}

		throw Exception("Couldn't find ID \"%d\" in list \"%s\"", id, instance().m_lists[0]->name());
	}

	Entry &Manager::findByName(const std::string &name)
	{
		return instance().m_lists[0]->find(name);
	}

	Entry &Manager::findByName(const std::string &name, List<Entry> *&outList)
	{
		for (size_t i = 1; i < instance().m_lists.size(); i++)
		{
			try
			{
				Entry &el = instance().m_lists[i]->find(name);

				outList = instance().m_lists[i];

				return el;
			}
			catch (Exception e) {}
		}

		throw Exception("Couldn't find name \"%s\" in list \"%s\"", name, instance().m_lists[0]->name());
	}
}
#include "ATN_Manager.h"

namespace ATN
{
	void Manager::addList(List<Entry> *list)
	{
		m_lists.push_back(list);
	}
	void Manager::removeEntry(const Entry &el)
	{
		m_lists[0]->remove(el);
	}

	const std::vector<List<Entry>*> Manager::lists() const
	{
		return m_lists;
	}

	void Manager::setAnims(List<Property> &list)
	{
		m_descAnims = list;
	}

	void Manager::setEvents(List<Property> &events)
	{
		m_descEvents = events;
	}

	Entry &Manager::findByID(std::uint32_t id) const
	{
		return m_lists[0]->find(id);
	}

	Entry &Manager::findByID(std::uint32_t id, List<Entry> *&outList) const
	{
		for (unsigned int i = 1; i < m_lists.size(); i++)
		{
			try
			{
				Entry &el = m_lists[i]->find(id);

				outList = m_lists[i];

				return el;
			}
			catch (Exception e) {}
		}

		throw Exception("Couldn't find ID \"%d\" in list \"%s\"", id, m_lists[0]->name());
	}

	Entry &Manager::findByName(const std::string &name) const
	{
		return m_lists[0]->find(name);
	}

	Entry &Manager::findByName(const std::string &name, List<Entry> *&outList) const
	{
		for (unsigned int i = 1; i < m_lists.size(); i++)
		{
			try
			{
				Entry &el = m_lists[i]->find(name);

				outList = m_lists[i];

				return el;
			}
			catch (Exception e) {}
		}

		throw Exception("Couldn't find name \"%s\" in list \"%s\"", name, m_lists[0]->name());
	}
}
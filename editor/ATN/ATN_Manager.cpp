#include "ATN_Manager.h"

namespace ATN
{
	Manager::Manager()
	{
		m_lists.push_back(new List<Entry>(":Global:"));
	}

	Manager::~Manager()
	{
		// As we hold all state references in mind, we will clean up any references left in memory when the application terminates

		delete m_lists[0];

		for (size_t i = 1; i < m_lists.size(); i++)
		{
			m_lists[i]->clear();

			delete m_lists[i];
		}
	}
	void Manager::loadFromFiles(const std::vector<std::string> &files)
	{
		std::vector<List<Entry>*> atns;

		for (const std::string &file : files)
		{
			List<Entry> *entries = new List<Entry>(file);

			atns.push_back(entries);

			util::parseATN(file, *entries, false);
		}

		for (size_t i = 0; i < files.size(); i++)
		{
			util::parseATN(files[i], *atns[i], true);
		}

		// Since names are added in the second pass, but the global ATN list required all IDs for that to run, we add the names afterwards here (for quick look-up)
		for (size_t i = 1; i < lists().size(); i++)
		{
			for (const std::pair<std::uint32_t, IATN_Data*> &pair : *lists()[i])
				lists()[0]->updateName(*(Entry*)pair.second);
		}
	}

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

	void Manager::setHashValues(const char * const strType, List<Property> &list)
	{
		instance().m_descValues[strType] = list;
	}

	const List<Property> &Manager::getHashValues(const char * const strType)
	{
		return instance().m_descValues[strType];
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

	std::vector<std::pair<Entry*, List<Entry>*>> Manager::searchName(const std::string &namePart)
	{
		// TODO
		return std::vector<std::pair<Entry*, List<Entry>*>>();
	}
}
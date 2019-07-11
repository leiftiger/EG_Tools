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

		// Also clear definitions
		for (const std::pair<std::string, List<Property>> &pair : m_descValues)
		{
			m_descValues.at(pair.first).clear();
		}
	}

	void Manager::addInterpretation(const std::string &type, const std::string &format)
	{
		instance().m_interpretationFormats[type] = format;
	}

	bool Manager::hasInterpretation(const std::string &type)
	{
		return instance().m_interpretationFormats.find(type) != instance().m_interpretationFormats.end();
	}

	const std::string & Manager::getInterpreration(const std::string &type)
	{
		return instance().m_interpretationFormats[type];
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
				lists()[0]->registerName(*(Entry*)pair.second);
		}
	}

	void Manager::addList(List<Entry> *list)
	{
		// Copy all elements in this list to the global ATN list
		for (const std::pair<std::uint32_t, IATN_Data*> &pair : *list)
		{
			instance().m_lists[0]->add(*(Entry*)pair.second);

			// Elixir-specified effects and percepts should all be captured here
			if (typeid(*pair.second) == typeid(Effect))
			{
				instance().m_effects.push_back((Effect*)pair.second);

				// Ensure these are always sorted
				std::sort(instance().m_effects.begin(), instance().m_effects.end(), compareLessThanPointersIATN);

				instance().m_effects.erase(std::unique(instance().m_effects.begin(), instance().m_effects.end(), compareEqualPointersIATN), instance().m_effects.end());
			}
			else if (typeid(*pair.second) == typeid(Percept))
			{
				instance().m_percepts.push_back((Percept*)pair.second);

				// Ensure these are always sorted (and clear duplicates)
				std::sort(instance().m_percepts.begin(), instance().m_percepts.end(), compareLessThanPointersIATN);

				instance().m_percepts.erase(std::unique(instance().m_percepts.begin(), instance().m_percepts.end(), compareEqualPointersIATN), instance().m_percepts.end());
			}
			else if (typeid(*pair.second) == typeid(Network))
			{
				instance().m_networks.push_back((Network*)pair.second);

				// Ensure these are always sorted
				std::sort(instance().m_networks.begin(), instance().m_networks.end(), compareLessThanPointersIATN);

				instance().m_networks.erase(std::unique(instance().m_networks.begin(), instance().m_networks.end(), compareEqualPointersIATN), instance().m_networks.end());
			}
		}

		instance().m_lists.push_back(list);
	}

	void Manager::addEntry(Entry &el)
	{
		instance().m_lists[0]->add(el);

		// This should never be called, but is here in case we do want to allow this in the future
		if (typeid(el) == typeid(Effect))
		{
			instance().m_effects.push_back((Effect*)&el);

			// Ensure these are always sorted
			std::sort(instance().m_effects.begin(), instance().m_effects.end(), compareLessThanPointersIATN);

			instance().m_effects.erase(std::unique(instance().m_effects.begin(), instance().m_effects.end(), compareEqualPointersIATN), instance().m_effects.end());
		}
		else if (typeid(el) == typeid(Percept))
		{
			instance().m_percepts.push_back((Percept*)&el);

			// Ensure these are always sorted
			std::sort(instance().m_percepts.begin(), instance().m_percepts.end(), compareLessThanPointersIATN);

			instance().m_percepts.erase(std::unique(instance().m_percepts.begin(), instance().m_percepts.end(), compareEqualPointersIATN), instance().m_percepts.end());
		}
		else if (typeid(el) == typeid(Network))
		{
			instance().m_networks.push_back((Network*)&el);

			// Ensure these are always sorted
			std::sort(instance().m_networks.begin(), instance().m_networks.end(), compareLessThanPointersIATN);

			instance().m_networks.erase(std::unique(instance().m_networks.begin(), instance().m_networks.end(), compareEqualPointersIATN), instance().m_networks.end());
		}
	}

	void Manager::removeEntry(const Entry &el)
	{
		// Because the remove call from the global list will inevitably call this function again,
		// we protect against further calls until the main one is finished
		if (instance().m_removingGlobalElement)
			return;

		instance().m_removingGlobalElement = true;

		instance().m_lists[0]->remove(el);

		instance().m_removingGlobalElement = false;
	}

	uint32_t Manager::maxID()
	{
		return instance().m_lists[0]->maxID();
	}

	const std::vector<List<Entry>*> Manager::lists()
	{
		return instance().m_lists;
	}

	void Manager::setDefinitions(const std::string &strType, List<Property> &list)
	{
		instance().m_descValues[strType] = list;
	}

	List<Property> &Manager::getDefinitions(const std::string &strType)
	{
		return instance().m_descValues[strType];
	}

	bool Manager::hasDefinitions(const std::string &strType)
	{
		return instance().m_descValues.find(strType) != instance().m_descValues.end();
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
		// TODO: improve to make search faster

		std::vector<std::pair<Entry*, List<Entry>*>> results;


		std::string lCaseSearch = namePart;

		std::transform(lCaseSearch.begin(), lCaseSearch.end(), lCaseSearch.begin(), ::tolower);


		for (size_t i = 1; i < instance().m_lists.size(); i++)
		{
			for (const std::pair<std::uint32_t, IATN_Data*> &pair : *instance().m_lists[i])
			{
				Entry *e = (Entry*)pair.second;

				std::string lCase = e->name();

				std::transform(lCase.begin(), lCase.end(), lCase.begin(), ::tolower);

				if (lCase.find(lCaseSearch) != -1)
					results.push_back(std::make_pair(e, instance().m_lists[i]));
			}
		}

		return results;
	}
	const std::vector<Effect*> &Manager::getEffects()
	{
		return instance().m_effects;
	}
	const std::vector<Percept*> &Manager::getPercepts()
	{
		return instance().m_percepts;
	}
	const std::vector<Network*> &Manager::getNetworks()
	{
		return instance().m_networks;
	}
}
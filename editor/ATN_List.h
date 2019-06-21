#pragma once

#include <vector>
#include <map>
#include <unordered_map>

#include "IATN_Data.h"

#include "ATN_Exception.h"

namespace ATN
{
	template <class T>
	class List
	{

	private:
		std::map<std::uint32_t, IATN_Data*> m_idMap;

		std::unordered_map<std::string, IATN_Data*> m_nameMap;

		std::string m_name;

		int m_numOrderNext = 0;
		int m_numOrderMax = INT_MAX;

		std::unordered_map<IATN_Data*, int> m_numOrder;

	public:

		// Ensures these pointers are always initialized correctly
		List<T>() {}

		List<T>(const std::string &name) : m_name(name) {}

		// Deletes the memory used by all elements in the list
		void clear()
		{
			for (std::pair<std::uint32_t, IATN_Data*> pair : this->m_idMap)
			{
				delete pair.second;
			}

			this->m_idMap.clear();
			this->m_nameMap.clear();
			this->m_numOrder.clear();
		}

		// Name of this ATN list for local references
		const std::string &name() const
		{
			return this->m_name;
		}

		// The maximum ObjectNum value of which order can be safely and easily preserved
		// Beyond this, we just save in an arbitrary order
		int maxPreservedNumOrder() const
		{
			return m_numOrderMax;
		}

		void add(const T &element)
		{
			m_idMap.insert(std::make_pair<std::uint32_t, IATN_Data*>(element.id(), (IATN_Data*)&element));

			m_nameMap[element.name()] = (IATN_Data*)&element;

			m_numOrder[(IATN_Data*)&element] = m_numOrderNext++;
		}

		void remove(const T &element)
		{
			m_idMap.erase(element.id());

			m_nameMap.erase(element.name());

			m_numOrderMax = m_numOrder[&element] >= m_numOrderMax ? m_numOrderMax : m_numOrder[&element]-1;
		}

		// Finds element by id, throws exception if it's not found
		T &find(std::uint32_t id)
		{
			std::map<std::uint32_t, IATN_Data*>::iterator it = m_idMap.find(id);

			if (it != m_idMap.end())
			{
				return *((T*)(it->second));
			}

			throw ATN::Exception("Couldn't find ID \"%d\" in list \"%s\"", id, this->m_name);
		}

		// Finds element by name, throws exception if it's not found
		T &find(const std::string &name)
		{
			std::unordered_map<std::string, IATN_Data*>::iterator it = m_nameMap.find(name);

			if (it != m_nameMap.end())
			{
				return *((T*)(it->second));
			}

			throw ATN::Exception("Couldn't find name \"%s\" in list \"%s\"", name, this->m_name);
		}
	};
}
#pragma once

#include <vector>
#include <map>
#include <unordered_map>

#include "IATN_Data.h"

namespace ATN
{
	template <class T>
	class List
	{

	private:
		std::map<std::uint32_t, IATN_Data*> m_idMap;

		std::unordered_map<std::string, IATN_Data*> m_nameMap;

		int m_numOrderNext = 0;
		int m_numOrderMax = INT_MAX;

		std::unordered_map<IATN_DATA*, int> m_numOrder;

	public:

		// Ensures these pointers are always initialized correctly
		List<T>() : m_numOrderNext(0), m_numOrderMax(INT_MAX)
		{

		}

		// The maximum ObjectNum value of which order can be safely and easily preserved
		// Beyond this, we just save in an arbitrary order
		int maxPreservedNumOrder() const
		{
			return m_numOrderMax;
		}

		void add(T &element)
		{
			m_idMap.insert(std::make_pair<std::uint32_t, IATN_Data*>(element.id(), (IATN_Data*)&element));

			m_nameMap[element.name()] = (IATN_Data*)&element;

			m_numOrder[&element] = m_numOrderNext++;
		}

		void remove(T &element)
		{
			m_idMap.erase(element.id());

			m_nameMap.erase(element.name());

			m_numOrderMax = m_numOrder[&element] >= m_numOrderMax ? m_numOrderMax : m_numOrder[&element]-1;
		}

		// Finds element by id, returns nullptr if it's not found
		T *find(std::uint32_t id)
		{
			std::map<std::uint32_t, IATN_Data*>::iterator it = m_idMap.find(id);

			if (it == m_idMap.end())
			{
				return it->second;
			}

			return nullptr;
		}

		// Finds element by name, returns nullptr if it's not found
		T *find(const std::string &name)
		{
			std::unordered_map<std::string, IATN_Data*>::iterator it = m_nameMap.find(name);

			if (it == m_nameMap.end())
			{
				return it->second;
			}

			return nullptr;
		}
	};
}
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
		std::unordered_map<std::uint32_t, IATN_Data*> m_idMap;

		std::unordered_map<std::string, IATN_Data*> m_nameMap;

		std::string m_name;

		int m_numOrderHeaderNext = 0;
		int m_numOrderHeaderMax = INT_MAX;

		int m_numOrderDataNext = 0;
		int m_numOrderDataMax = INT_MAX;

		std::unordered_map<IATN_Data*, int> m_numOrderHeader;
		std::unordered_map<IATN_Data*, int> m_numOrderData;

	public:
		typedef std::unordered_map<std::uint32_t, IATN_Data*>::iterator iterator;
		typedef std::unordered_map<std::uint32_t, IATN_Data*>::const_iterator const_iterator;

		iterator begin()
		{
			return m_idMap.begin();
		}

		iterator end()
		{
			return m_idMap.end();
		}

		const_iterator cbegin()
		{
			return m_idMap.cbegin();
		}

		const_iterator cend()
		{
			return m_idMap.cend();
		}

		// Ensures these pointers are always initialized correctly
		List<T>() {}

		List<T>(const std::string &name) : m_name(name) {}

		// Deletes the memory used by all elements in the list
		void clear()
		{
			for (std::pair<std::uint32_t, IATN_Data*> pair : m_idMap)
			{
				delete pair.second;
			}

			m_idMap.clear();
			m_nameMap.clear();
			m_numOrderHeader.clear();
			m_numOrderData.clear();
		}

		// Name of this ATN list for local references
		const std::string &name() const
		{
			return m_name;
		}

		// The maximum ObjectNum value of which order can be safely and easily preserved
		// Beyond this, we just save in an arbitrary order
		int maxPreservedNumOrderHeader() const
		{
			return m_numOrderHeaderMax;
		}

		void add(const T &element)
		{
			m_idMap.insert(std::make_pair<std::uint32_t, IATN_Data*>(element.id(), (IATN_Data*)&element));

			updateName(element);
		}

		void updateName(const T &element)
		{
			m_nameMap[element.name()] = (IATN_Data*)&element;
		}

		// Note where this entry appeared in the header list
		void recordOrderHeader(const T &element)
		{
			m_numOrderHeader[(IATN_Data*)&element] = m_numOrderHeaderNext++;
		}

		// Note where this entry appeared in the data list
		void recordOrderData(const T &element)
		{
			m_numOrderData[(IATN_Data*)&element] = m_numOrderDataNext++;
		}

		void remove(const T &element)
		{
			m_idMap.erase(element.id());

			m_nameMap.erase(element.name());

			m_numOrderHeaderMax = m_numOrderHeader[(IATN_Data*)&element] >= m_numOrderHeaderMax ? m_numOrderHeaderMax : m_numOrderHeader[(IATN_Data*)&element]-1;
			m_numOrderDataMax = m_numOrderData[(IATN_Data*)&element] >= m_numOrderDataMax ? m_numOrderDataMax : m_numOrderData[(IATN_Data*)&element] - 1;
		}

		// Finds element by id, throws exception if it's not found
		T &find(std::uint32_t id)
		{
			iterator it = m_idMap.find(id);

			if (it != m_idMap.end())
			{
				return *((T*)(it->second));
			}

			throw ATN::Exception("Couldn't find ID \"%d\" in list \"%s\"", id, m_name);
		}

		// Finds element by name, throws exception if it's not found
		T &find(const std::string &name)
		{
			std::unordered_map<std::string, IATN_Data*>::iterator it = m_nameMap.find(name);

			if (it != m_nameMap.end())
			{
				return *((T*)(it->second));
			}

			throw ATN::Exception("Couldn't find name \"%s\" in list \"%s\"", name, m_name);
		}
	};
}
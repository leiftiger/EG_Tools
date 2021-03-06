#pragma once

#include <climits>
#include <map>
#include <unordered_map>
#include <vector>

#include "IATN_Data.h"
#include "ATN_Property.h"

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

		uint32_t m_maxUniqueID = 0;

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
		void clear(bool freeMemory = true)
		{
			if (freeMemory)
			{
				for (const std::pair<std::uint32_t, IATN_Data*> &pair : m_idMap)
				{
					delete pair.second;
				}
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

		// Returns the maximum ID noted to appear in this list
		uint32_t maxID() const
		{
			return m_maxUniqueID;
		}

		// Returns the number of entries stored in this list
		size_t size() const
		{
			return m_idMap.size();
		}

		// Add a new entry to this list
		void add(T &element)
		{
			m_idMap.insert_or_assign(element.id(), &element);

			if (m_maxUniqueID < (std::uint32_t)element.id())
				m_maxUniqueID = (std::uint32_t)element.id();

			registerName(element);
		}

		// Registers the entry's name for quick lookup in the list
		void registerName(const T &element)
		{
			m_nameMap[element.name()] = (IATN_Data*)&element;
		}

		// Sets the name of the entry to the specified name, updating the references in the list
		void updateName(T &element, const std::string &newName)
		{
			m_nameMap.erase(element.name());

			element.setName(newName);

			registerName(element);
		}

		// Note where this entry appeared in the header list - used for preserving write order
		void recordOrderHeader(const T &element)
		{
			m_numOrderHeader[(IATN_Data*)&element] = m_numOrderHeaderNext++;
		}

		// Note where this entry appeared in the data list - used for preserving write order
		void recordOrderData(const T &element)
		{
			m_numOrderData[(IATN_Data*)&element] = m_numOrderDataNext++;
		}

		// Returns an ordered list of how this ATN list should be written as object headers
		std::vector<T*> getWriteOrderHeader() const
		{
			std::vector<T*> order(size(), nullptr);

			std::vector<T*> unordered;

			for (const std::pair<std::uint32_t, IATN_Data*> &pair : this->m_idMap)
			{
				std::unordered_map<IATN_Data*, int>::const_iterator it = m_numOrderHeader.find(pair.second);

				int id = -1;

				if (it != m_numOrderHeader.end())
					id = (*it).second;

				if (id != -1 && id <= m_numOrderHeaderMax)
				{
					order[id] = (T*)pair.second;
				}
				else
				{
					unordered.push_back((T*)pair.second);
				}
			}

			size_t unorderedIndex = size() - unordered.size();

			// Add the unordered elements in a random order
			for (T *el : unordered)
			{
				order[unorderedIndex++] = el;
			}

			return order;
		}

		// Returns an ordered list of how this ATN list should be written as object data
		std::vector<T*> getWriteOrderData() const
		{
			std::vector<T*> order(size(), nullptr);

			std::vector<T*> unordered;

			for (const std::pair<std::uint32_t, IATN_Data*> &pair : this->m_idMap)
			{
				std::unordered_map<IATN_Data*, int>::const_iterator it = m_numOrderData.find(pair.second);

				int id = -1;

				if (it != m_numOrderData.end())
					id = (*it).second;

				if (id != -1 && id <= m_numOrderDataMax)
				{
					order[id] = (T*)pair.second;
				}
				else
				{
					unordered.push_back((T*)pair.second);
				}
			}

			size_t unorderedIndex = size() - unordered.size();

			// Add the unordered elements in a random order
			for (T *el : unordered)
			{
				order[unorderedIndex++] = el;
			}

			return order;
		}

		// Removes an entry from this list and clears it from the preserved write order
		void remove(const T &element)
		{
			m_idMap.erase(element.id());

			m_nameMap.erase(element.name());

			if (m_numOrderHeader.find((IATN_Data*)&element) != m_numOrderHeader.end())
			{
				int orderHeader = m_numOrderHeader[(IATN_Data*)&element];
				int orderData = m_numOrderData[(IATN_Data*)&element];

				// Since this index is now unoccupied, move all orders down
				for (const std::pair<std::uint32_t, IATN_Data*> &pair : this->m_idMap)
				{
					std::unordered_map<IATN_Data*, int>::const_iterator itHeader = m_numOrderHeader.find(pair.second);
					std::unordered_map<IATN_Data*, int>::const_iterator itData = m_numOrderData.find(pair.second);

					if (itHeader != m_numOrderHeader.end())
					{
						if (orderHeader < (*itHeader).second)
							m_numOrderHeader[pair.second] -= 1;
						if (orderData < (*itData).second)
							m_numOrderData[pair.second] -= 1;
					}
				}
			}
		}

		// Returns true if this list contains an element with the given ID
		bool contains(std::uint32_t id) const
		{
			return (m_idMap.find(id) != m_idMap.end());
		}

		// Returns true if this list contains an element with the given name
		bool contains(const std::string &name) const
		{
			return (m_nameMap.find(name) != m_nameMap.end());
		}

		// Finds element by id, throws exception if it's not found
		T &find(std::uint32_t id) const
		{
			const_iterator it = m_idMap.find(id);

			if (it != m_idMap.end())
			{
				return *((T*)(it->second));
			}

			throw ATN::Exception("Couldn't find ID \"%d\" in list \"%s\"", id, m_name);
		}

		// Finds element by name, throws exception if it's not found
		T &find(const std::string &name) const
		{
			std::unordered_map<std::string, IATN_Data*>::const_iterator it = m_nameMap.find(name);

			if (it != m_nameMap.end())
			{
				return *((T*)(it->second));
			}

			throw ATN::Exception("Couldn't find name \"%s\" in list \"%s\"", name, m_name);
		}
	};
}
#pragma once

#include <cstdint>
#include <iostream>
#include <regex>
#include <vector>
#include <string>

#include "ATN_Entry.h"
#include "ATN_List.h"
#include "ATN_Manager.h"
#include "ATN_Property.h"

// Forward declared because of circular dependency
namespace ATN
{
	template <class T>
	class List;
}

namespace util
{
	extern int DEBUG_LINE;

	// Hash used for animations and event names
	std::int32_t hashElixir(const std::string &str);

	// Proxy to std::getline that also keeps track of the line number of the currently open file
	std::istream &getline(std::istream &stream, std::string &line);

	// Reads all events
	ATN::List<ATN::Property> parseHashes(const std::string &filename);

	ATN::List<ATN::Property> createDefinition(std::initializer_list<std::pair<std::string, std::int32_t>> list);

	// Parse all entries from specified file
	void parseATN(const std::string &filename, ATN::List<ATN::Entry> &outList, bool secondPass);

	// Write all ATN entries to specified file
	void writeATN(const std::string &filename, const ATN::List<ATN::Entry> &list);

	// Parse a series of entry IDs from an ATN entry
	template <class T>
	std::vector<T*> parseEntryIDs(std::istream &stream, const char * const header)
	{
		std::string line;

		getline(stream, line);

		int numEntries = std::stoi(line.substr(strlen(header)));

		getline(stream, line);

		std::vector<T*> entries;

		if (numEntries > 0)
		{
			const std::regex regStr("([0-9]+) ");

			std::smatch matches;

			std::string subLine = line;

			for (int i = 0; i < numEntries; i++)
			{
				if (!std::regex_search(subLine, matches, regStr))
				{
					throw ATN::Exception("Missing %d IDs in \"%s\"", numEntries - i, line);
				}
				else
				{
					uint32_t id = std::stoi(matches[1]);

					entries.push_back((T*)&ATN::Manager::findByID(id));

					subLine = matches.suffix();
				}
			}
		}

		return entries;
	}

	// Write a series of numbers to an ATN entry
	template <class T>
	void writeEntryIDs(std::ostream &stream, const char * const header, const std::vector<T*> &entries)
	{
		stream << header << entries.size() << std::endl;

		stream << "{ ";

		for (T *entry : entries)
		{
			stream << entry->id() << " ";
		}

		stream << "}" << std::endl;
	}
}
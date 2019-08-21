#pragma once

#include <cstdint>
#include <iostream>
#include <regex>
#include <vector>
#include <string>

#include "ATN/ATN_Entry.h"
#include "ATN/ATN_List.h"
#include "ATN/ATN_Manager.h"
#include "ATN/ATN_Property.h"

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
	std::int32_t hashFNV132(const std::string &str);

	// Proxy to std::getline that also keeps track of the line number of the currently open file and clears the carriage return if it exists
	std::istream &getline(std::istream &stream, std::string &line);

	// Gets the config paths set by the config file
	std::vector<std::string> configPaths(const std::string &filename);

	// Reads all events
	ATN::List<ATN::Property> parseHashes(const std::string &filename);

	ATN::List<ATN::Property> createDefinition(const std::initializer_list<std::pair<std::string, std::int32_t>> &list);
	ATN::List<ATN::Property> createDefinition(const std::vector<std::pair<std::string, std::int64_t>> &list);

	std::vector<std::pair<std::string, std::string>> parseInterpretations(const std::string &filename);

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
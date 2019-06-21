#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#include "ATN_Entry.h"
#include "ATN_List.h"
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
	ATN::List<ATN::Property> parseEvents(const std::string &filename);

	// Parse all entries from specified file
	void parseATN(const std::string &filename, ATN::List<ATN::Entry> &outList, bool secondPass);
}
#pragma once

#include <cstdint>
#include <string>

#include "ATN_Entry.h"
#include "ATN_List.h"
#include "ATN_Property.h"

namespace util
{
	// Hash used for animations and event names
	std::uint32_t hashElixir(const std::string &str);

	// Reads all events
	ATN::List<ATN::Property> parseEvents(const std::string &filename);

	// Parse all entries from specified file
	ATN::List<ATN::Entry> parseATN(const std::string &filename);
}
#pragma once

#include <string>
#include <cstdint>

namespace ATN
{
	// Interface class to ease look ups for certain IDs and events
	class IATN_Data
	{
	public:
		// Unique identifier of this ATN entry / event
		virtual uint32_t id() const = 0;
		// Human-readable name of this ATN entry / event
		virtual const std::string & name() const = 0;

		friend bool operator<(const IATN_Data &a, const IATN_Data &b);
	};
}
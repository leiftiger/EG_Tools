#pragma once
#pragma GCC diagnostic ignored "-Wignored-qualifiers" // We know that virtual overrides don't need virtual, but it's nicer that way!

#include <string>
#include <cstdint>

namespace ATN
{
	// Interface class to ease look ups for certain IDs and events
	class IATN_Data
	{
	public:

		virtual ~IATN_Data() = default;

		// Unique identifier of this ATN entry / event
		virtual int32_t id() const = 0;
		// Human-readable name of this ATN entry / event
		virtual const std::string &name() const = 0;

		friend bool operator<(const IATN_Data &a, const IATN_Data &b);

		friend bool compareLessThanPointersIATN(const IATN_Data *a, const IATN_Data *b);

		friend bool compareEqualPointersIATN(const IATN_Data *a, const IATN_Data *b);
	};
}
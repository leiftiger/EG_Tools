#include "ATN_State.h"

namespace ATN
{
	const char * const State::typeName() const
	{
		return "TATNState";
	}

	void State::serialize(std::ostream & stream) const
	{

	}

	void State::deserialize(std::istream & stream)
	{
		std::string line;

		// PLACEHOLDER, read until end of object to skip this class for now
		while (util::getline(stream, line))
		{
			if (line == "")
				break;
		}
	}
}
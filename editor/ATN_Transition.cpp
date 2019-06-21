#include "ATN_Transition.h"

namespace ATN
{
	const char * const Transition::typeName() const
	{
		return "TATNStateTransition";
	}

	void Transition::serialize(std::ostream &stream) const
	{

	}

	void Transition::deserialize(std::istream &stream)
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
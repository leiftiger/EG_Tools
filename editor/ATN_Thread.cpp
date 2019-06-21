#include "ATN_Thread.h"

namespace ATN
{
	const char * const Thread::typeName() const
	{
		return "TATNThread";
	}

	void Thread::serialize(std::ostream &stream) const
	{

	}

	void Thread::deserialize(std::istream &stream)
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
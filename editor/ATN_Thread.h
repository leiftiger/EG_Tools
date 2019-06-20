#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	// A thread, it serves as the first point of entry to a sequence of states
	class Thread : protected Entry
	{
		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
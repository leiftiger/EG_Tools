#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	class Transition : protected Entry
	{
		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
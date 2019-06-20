#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	class State : protected Entry
	{
		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
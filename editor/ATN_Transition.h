#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	class Transition : protected Entry
	{
		virtual const char * const typeName() const override;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
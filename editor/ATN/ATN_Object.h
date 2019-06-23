#pragma once

#include "ATN_Entry.h"
#include "ATN_State.h"

namespace ATN
{
	// An object type, appears to be a remnant and not really necessary, but is included for completion's sake
	class Object : public Entry
	{
	protected:
		State *m_parent = nullptr;

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;
	public:

		virtual const char * const typeName() const override;
	};
}
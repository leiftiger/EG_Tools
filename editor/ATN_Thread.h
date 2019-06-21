#pragma once

#include "ATN_Entry.h"
#include "ATN_State.h"
#include "ATN_Network.h"

namespace ATN
{
	// Forward declaration
	class State;  class Network;

	// A thread, it serves as the first point of entry to a sequence of states
	class Thread : public Entry
	{
	protected:

		// Network thread belongs to
		Network *m_network = nullptr;
		// Initial state in thread
		State *m_state = nullptr;

		virtual const char * const typeName() const override;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
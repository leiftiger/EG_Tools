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

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;

	public:

		virtual const char * const typeName() const override;

		// Gets the network this thread belongs to
		const Network &network() const;

		// Sets the network this thread belongs to
		void setNetwork(Network &network);

		// Gets the state this thread goes to
		const State *state() const;

		// Sets the state this thread goes to
		void setState(State *state);
	};
}
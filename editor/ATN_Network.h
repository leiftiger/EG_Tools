#pragma once

#include "ATN_Entry.h"
#include "ATN_State.h"
#include "ATN_Thread.h"
#include "ATN_Resources.h"

namespace ATN
{
	// Forward declared
	class State; class Thread; class Transition;

	// A network, it serves as a container from which states can access its resources and send it parameters (?)
	class Network : public Entry
	{
	protected:

		// The states that will be traversed in the network
		std::vector<State*> m_states;
		std::vector<Thread*> m_threads;

		std::vector<Resource> m_resources;
		std::vector<Parameter> m_parameters;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;

	public:

		virtual const char * const typeName() const override;
	};
}

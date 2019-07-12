#pragma once

#include "ATN_Entry.h"
#include "ATN_Transition.h"
#include "ATN_Network.h"
#include "ATN_Resources.h"

namespace ATN
{
	// Forward declared
	class Network; class Transition;

	class State : public Entry
	{
		// The network needs to modify some of the marshalls if its parameters or resources are swapped
		friend class Network;

	protected:

		std::vector<Transition*> m_stateTransitions;

		Network *m_networkTransition = nullptr;

		std::vector<ResourceMarshall*> m_resourceMarshalls;
		std::vector<ParameterMarshall*> m_parameterMarshalls;
		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;

	public:

		~State();

		virtual const char * const typeName() const override;

		// Gets the possible transitions from this state
		const std::vector<Transition*> &transitions() const;

		// Add a transition from this state to be evaluated last
		void add(Transition &t);
		// Move specified transition higher in evaluation order
		void moveUp(Transition &t);
		// Move specified transition lower in evaluation order
		void moveDown(Transition &t);
		// Remove specified transition from this state
		std::vector<Transition*>::iterator remove(Transition &t);

		// Gets the network transition call from this state if set
		const Network *networkTransition() const;
		// Sets the network transition call from this state, recreating resource and parameter marshalls to suit new call
		void setNetworkTransition(Network *net);

		// Resource arguments sent in network transition, set automatically based on network transition
		const std::vector<ResourceMarshall*> &resourceMarshalls() const;

		// Parameter arguments sent in network transition, set automatically based on network transition
		const std::vector<ParameterMarshall*> &parameterMarshalls() const;
	};
}
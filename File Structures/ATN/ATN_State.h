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
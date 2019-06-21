#pragma once

#include "ATN_Entry.h"
#include "ATN_Transition.h"
#include "ATN_Network.h"
#include "ATN_Resources.h"

namespace ATN
{
	// Forward declared
	class Network;

	class State : public Entry
	{
	protected:

		std::vector<Transition*> m_stateTransitions;

		Network *m_networkTransition = nullptr;

		std::vector<ResourceMarshall> m_resourceMarshalls;
		std::vector<ParameterMarshall> m_parameterMarshalls;
		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;

	public:

		virtual const char * const typeName() const override;
	};
}
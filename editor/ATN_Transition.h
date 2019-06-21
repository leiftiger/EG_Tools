#pragma once

#include "ATN_Effect.h"
#include "ATN_Entry.h"
#include "ATN_Percept.h"
#include "ATN_State.h"
#include "ATN_Resources.h"

namespace ATN
{
	// Forward declaration
	class State;

	class Transition : public Entry
	{
	protected:

		// Effect to perform if transition is taken
		Effect *m_effect = nullptr;

		std::vector<ResourceMarshall> m_effectResources;
		std::vector<ParameterMarshall> m_effectParameters;

		// Percept to check if this transition should be taken
		Percept *m_percept = nullptr;

		std::vector<ResourceMarshall> m_perceptResources;
		std::vector<ParameterMarshall> m_perceptParameters;

		// State to transition to if transition is taken
		State *m_state = nullptr;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;

	public:

		virtual const char * const typeName() const override;
	};
}
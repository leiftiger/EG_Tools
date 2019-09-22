#pragma once

#include "ATN_Effect.h"
#include "ATN_Entry.h"
#include "ATN_Percept.h"
#include "ATN_State.h"
#include "ATN_Resources.h"

namespace ATN
{
	// Forward declared
	class State; class Effect; class Percept; class ResourceMarshall; class ParameterMarshall;

	class Transition : public Entry
	{
	protected:

		// Effect to perform if transition is taken
		Effect *m_effect = nullptr;

		std::vector<ResourceMarshall*> m_effectResources;
		std::vector<ParameterMarshall*> m_effectParameters;

		// Percept to check if this transition should be taken
		Percept *m_percept = nullptr;

		std::vector<ResourceMarshall*> m_perceptResources;
		std::vector<ParameterMarshall*> m_perceptParameters;

		// State to transition to if transition is taken
		State *m_state = nullptr;

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;
		virtual bool equals(const Entry *other) const override;

	public:

		~Transition();

		virtual const char * const typeName() const override;

		virtual void applyChanges(const Entry &original, const Entry &change) override;

		// Get effect to perform if transition is taken
		Effect *effect() const;

		// Set effect to perform if transition is taken, recreating resource and parameter marshalls to suit new call
		void setEffect(Effect *effect);

		// Resource arguments sent to effect, populated from current set effect
		const std::vector<ResourceMarshall*> &effectResourceMarshalls() const;

		// Parameter arguments sent to effect, populated from current set effect
		const std::vector<ParameterMarshall*> &effectParameterMarshalls() const;

		// Get percept to check if this transition should be taken
		Percept *percept() const;

		// Set percept to check if this transition should be taken, recreating resource and parameter marshalls to suit new call
		void setPercept(Percept *percept);

		// Resource arguments sent to percept, populated from current set percept
		const std::vector<ResourceMarshall*> &perceptResourceMarshalls() const;

		// Parameter arguments sent to percept, populated from current set percept
		const std::vector<ParameterMarshall*> &perceptParameterMarshalls() const;

		// Get state to transition to if transition is taken
		const State *state() const;

		// Set state to transition to if transition is taken
		void setState(State *state);

		// Copy all properties except ID and state from another transition
		void copyFrom(const Transition *other);
	};
}
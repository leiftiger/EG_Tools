#pragma once

#include "ATN_Entry.h"
#include "ATN_Transition.h"

namespace ATN
{
	class State : public Entry
	{
	protected:

		std::vector<Transition*> m_stateTransitions;

		Network *m_networkTransition = nullptr;

		virtual const char * const typeName() const override;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
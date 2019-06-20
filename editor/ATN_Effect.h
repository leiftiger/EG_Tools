#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	// An action to the game engine, uneditable
	class Effect : protected Entry
	{
	private:
		std::string m_gameFunction;
	public:

		Effect(std::string &gameFunction);

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		const std::string &gameFunction() const;

		void setGameFunction(std::string &gameFunction);

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
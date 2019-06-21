#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	// "if" condition in the ATN, uneditable
	class Percept : protected Entry
	{
	private:
		std::string m_gameFunction;
	public:

		Percept(std::string &gameFunction);

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		const std::string &gameFunction() const;

		void setGameFunction(std::string &gameFunction);

		virtual const char * const typeName() const override;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
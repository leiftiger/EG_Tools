#pragma once

#include "ATN_Entry.h"
#include "ATN_Resources.h"

namespace ATN
{
	// "if" condition in the ATN, uneditable
	class Percept : public Entry
	{
	private:

		std::string m_gameFunction;

		// Just like so-called DNA junk, this thing is important, but we don't know what it does
		std::string m_graphJunk;

		std::vector<Resource> m_resources;
		std::vector<Parameter> m_parameters;

	protected:

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;

	public:

		Percept(std::string &gameFunction);

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		const std::string &gameFunction() const;

		void setGameFunction(std::string &gameFunction);

		virtual const char * const typeName() const override;

	};
}
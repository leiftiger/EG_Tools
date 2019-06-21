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

		// stored to be able to properly restore it again
		std::string m_graphJunk;

		std::vector<Resource> m_resources;
		std::vector<Parameter> m_parameters;

	protected:

		virtual const char * const typeName() const override;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;

	public:

		Percept(std::string &gameFunction);

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		const std::string &gameFunction() const;

		void setGameFunction(std::string &gameFunction);
	};
}
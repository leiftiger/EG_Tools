#pragma once

#include "ATN_Entry.h"
#include "ATN_Resources.h"

namespace ATN
{
	// An action to the game engine, uneditable
	class Effect : public Entry
	{
	private:
		std::string m_gameFunction;

		// Resources affected by this effect
		std::vector<Resource> m_resources;
		// Input parameters to this effect
		std::vector<Parameter> m_parameters;
	public:

		Effect(std::string &gameFunction);

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		const std::string &gameFunction() const;

		void setGameFunction(std::string &gameFunction);

		virtual const char * const typeName() const override;

		virtual void serialize(std::ostream & stream) const override;
		virtual void deserialize(std::istream & stream) override;
	};
}
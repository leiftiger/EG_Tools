#pragma once

#include "ATN_Entry.h"
#include "ATN_IResourceHolder.h"
#include "ATN_Resources.h"

namespace ATN
{
	// An action to the game engine, uneditable
	class Effect : public Entry, IResourceHolder
	{
	protected:

		std::string m_gameFunction;

		// Resources affected by this effect
		std::vector<Resource*> m_resources;
		// Input parameters to this effect
		std::vector<Parameter*> m_parameters;

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;
		virtual bool equals(const Entry *other) const override;

	public:

		Effect(std::string &gameFunction);
		~Effect();

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		// Executable game function that this effect calls
		const std::string &gameFunction() const;

		// Sets the executable game function that this effect calls
		void setGameFunction(std::string &gameFunction);

		virtual const char * const typeName() const override;

		// Gets all parameters expected by this effect
		virtual const std::vector<Parameter*> &parameters() const override;
		// Gets all resources expected by this percept
		virtual const std::vector<Resource*> &resources() const override;
	};
}
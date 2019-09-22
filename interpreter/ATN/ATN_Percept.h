#pragma once

#include "ATN_Entry.h"
#include "ATN_IResourceHolder.h"
#include "ATN_Resources.h"

namespace ATN
{
	// "if" condition in the ATN, uneditable
	class Percept : public Entry, IResourceHolder
	{
	private:

		std::string m_gameFunction;

		// Just like so-called DNA junk, this thing is important, but we don't know what it does
		std::string m_graphJunk;

		std::vector<Resource*> m_resources;
		std::vector<Parameter*> m_parameters;

	protected:

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;
		virtual bool equals(const Entry *other) const override;

	public:

		Percept(std::string &gameFunction);
		~Percept();

		// NOT EDITABLE FOR YOUR OWN SAFETY
		virtual bool isEditable() const override;

		virtual const char * const typeName() const override;

		virtual void applyChanges(const Entry &original, const Entry &change) override;

		// Executable game function that this percept calls
		const std::string &gameFunction() const;

		// Sets the executable game function that this percept calls
		void setGameFunction(std::string &gameFunction);

		// Gets all parameters expected by this percept
		virtual const std::vector<Parameter*> &parameters() const override;

		// Gets all resources expected by this percept
		virtual const std::vector<Resource*> &resources() const override;
	};
}
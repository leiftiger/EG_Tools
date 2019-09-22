#pragma once

#include "ATN_Entry.h"

namespace ATN
{
	// An object type, used to parse resource type hierarchies (i.e. EntityGroup <- Entity)
	class Object : public Entry
	{
	protected:
		Object *m_parent = nullptr;

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;
		virtual bool equals(const Entry *other) const override;


	public:

		virtual const char * const typeName() const override;

		virtual void applyChanges(const Entry &original, const Entry &change);

		// Gets the parent of this resource type
		const Object *parent() const;
	};
}
#pragma once

#include "IATN_Data.h"

#include <vector>
#include <iostream>

#include "ATN_DeltaPatching.h"

namespace ATN
{
	class Entry : public IATN_Data
	{
	private:
		std::int32_t m_id;
		std::string m_name;

	protected:

		// Converts this object into an ATN string entry (not including object header)
		virtual void serialize(std::ostream &stream) const;

		// Load in the properties of this object from an ATN string entry, read to the blank line
		virtual void deserialize(std::istream &stream);

		// Checks whether this entry is identical to the other one
		virtual bool equals(const Entry *other) const;

	public:

		virtual std::int32_t id() const override;
		virtual const std::string &name() const override;

		// ATN type name of this class
		virtual const char * const typeName() const = 0;

		// Applies a delta update corresponding to the difference between the original and changed entry to this object
		virtual void applyChanges(const Entry &original, const Entry &change, DeltaMemory &memory);

		// Sets the user-readable name of this entry.
		// If this entry is already in a list, you must call List<Entry>.updateName() instead!
		virtual void setName(const std::string &name);

		// Sets the Unique ID of this entry - ensure that an ID is set before adding it to a list!
		virtual void setID(std::int32_t id);

		// Whether or not this entry can be edited by the user (editing it may cause adverse game crashing effects!)
		virtual bool isEditable() const;

		// Converts this object into an ATN string entry (not including object header)
		friend std::ostream &operator<<(std::ostream &stream, const Entry &e);
		// Load in the properties of this object from an ATN string entry, read to the blank line
		friend std::istream &operator>>(std::istream &stream, Entry &e);

		// Compare whether an entry is identical to another (including subclasses)
		friend bool operator==(const Entry &lhs, const Entry &rhs);
		friend bool operator!=(const Entry &lhs, const Entry &rhs);
	};
}
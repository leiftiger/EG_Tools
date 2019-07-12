#pragma once

#include "IATN_Data.h"

#include <vector>
#include <iostream>

namespace ATN
{
	class Entry : public IATN_Data
	{
	private:
		std::int32_t m_id;
		std::string m_name;

		std::string *m_file = nullptr;

	protected:

		// Converts this object into an ATN string entry (not including object header)
		virtual void serialize(std::ostream &stream) const;

		// Load in the properties of this object from an ATN string entry, read to the blank line
		virtual void deserialize(std::istream &stream);

	public:

		// ATN type name of this class
		virtual const char * const typeName() const = 0;

		virtual std::int32_t id() const override;
		virtual const std::string &name() const override;

		// If this entry is already in a list, you must call List<Entry>.updateName() instead!
		virtual void setName(const std::string &name);
		// Do not call before putting this entry in a list!
		virtual void setID(std::int32_t id);

		// Whether or not this entry can be edited by the user (editing it may cause adverse game crashing effects!)
		virtual bool isEditable() const;

		// Converts this object into an ATN string entry (not including object header)
		friend std::ostream &operator<<(std::ostream &stream, const Entry &e);
		// Load in the properties of this object from an ATN string entry, read to the blank line
		friend std::istream &operator>>(std::istream &stream, Entry &e);
	};
}

// Included after the class definition to avoid circular dependencies
#include "utils.h"
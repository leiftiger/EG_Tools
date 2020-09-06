#include "ATN_Entry.h"

#include "utils.h"

namespace ATN
{
	void Entry::serialize(std::ostream &stream) const
	{
		stream << "TypeName=" << this->typeName() << std::endl;
		stream << "UniqueID=" << this->id() << std::endl;
		stream << "Name=" << this->name() << std::endl;
	}

	void Entry::deserialize(std::istream &stream)
	{
		// TypeName and UniqueID were already read to identify this entry
		std::string line;

		util::getline(stream, line);

		line = line.substr(strlen("Name="));

		setName(line);
	}

	bool Entry::equals(const Entry *other) const
	{
		if (this->id() != other->id())
			return false;

		if (this->name() != other->name())
			return false;

		// Not using typename since it may return different C strings for effects & percepts
		if (typeid(*this) != typeid(*other))
			return false;

		return true;
	}

	std::int32_t Entry::id() const
	{
		return this->m_id;
	}

	const std::string &Entry::name() const
	{
		return this->m_name;
	}

	void Entry::setName(const std::string &name)
	{
		this->m_name = name;
	}

	void Entry::setID(std::int32_t id)
	{
		this->m_id = id;
	}

	bool Entry::isEditable() const
	{
		return true;
	}

	void Entry::applyChanges(const Entry &original, const Entry &change, DeltaMemory &memory)
	{
		if (original.id() != change.id() || this->id() != original.id())
			throw ATN::Exception("applyChanges requires that all entries share the same unique ID");

		// All changes should be dealt with in the deriving classes, here we just do a type checking that all derived ones assume
		if (typeid(original) != typeid(change))
			throw ATN::Exception("Cannot apply changes from different classes, %s != %s", original.typeName(), change.typeName());

		if (original.name() != change.name())
		{
			if (this->name() == original.name())
				this->setName(change.name());
		}
	}

	std::ostream &operator<<(std::ostream &stream, const Entry &e)
	{
		e.serialize(stream);

		// Signify end of object
		stream << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, Entry &e)
	{
		e.deserialize(stream);

		std::string line;

		// blank line - we reached the end of this object
		util::getline(stream, line);

		if (line.length() != 0)
			throw Exception("Expected end of object, got \"%s\"", line);

		return stream;
	}

	bool operator==(const Entry &lhs, const Entry &rhs)
	{
		return (&lhs)->equals(&rhs);
	}

	bool operator!=(const Entry &lhs, const Entry &rhs)
	{
		return !(lhs == rhs);
	}
}
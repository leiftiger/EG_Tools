#include "ATN_Entry.h"

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
		std::string line;

		util::getline(stream, line);

		line = line.substr(strlen("Name="));

		setName(line);
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
}
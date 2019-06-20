#include "ATN_Entry.h"

namespace ATN
{
	std::uint32_t Entry::id() const
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

	void Entry::setID(std::uint32_t id)
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

		return stream;
	}

	std::istream &operator>>(std::istream &stream, Entry &e)
	{
		e.deserialize(stream);

		return stream;
	}
}
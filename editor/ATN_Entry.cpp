#include "ATN_Entry.h"

namespace ATN
{
	Entry::Entry()
	{

	}


	Entry::~Entry()
	{

	}

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
}
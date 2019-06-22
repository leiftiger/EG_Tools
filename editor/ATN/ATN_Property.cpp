#include "ATN_Property.h"

namespace ATN
{
	Property::Property(std::string name, std::uint32_t hash) : m_name(name), m_hash(hash)
	{
	}

	std::uint32_t Property::id() const
	{
		return m_hash;
	}

	const std::string & Property::name() const
	{
		return m_name;
	}
}
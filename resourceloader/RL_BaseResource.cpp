#include "RL_BaseResource.h"

namespace RL
{
	BaseResource::BaseResource(const std::string &type, const std::string &key, std::int64_t value) : m_type(type), m_key(key), m_value(value)
	{

	}

	const std::string &BaseResource::type() const
	{
		return m_type;
	}

	const std::string &BaseResource::key() const
	{
		return m_key;
	}

	std::int64_t BaseResource::value() const
	{
		return m_value;
	}
}
#include "ATN_Network.h"

namespace ATN
{
	Network::Parameter::Parameter(std::string type, std::int32_t defaultValue, std::string desc) : m_type(type), m_defaultValue(defaultValue), m_desc(desc)
	{

	}

	Network::Resource::Resource(ResourceType type, std::string desc, std::int32_t defaultValue) : m_type(type), m_desc(desc), m_defaultValue(defaultValue)
	{

	}
}
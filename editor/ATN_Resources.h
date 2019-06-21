#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

namespace ATN
{
	enum class ParameterMarshallType : int
	{
		Unknown = 0,
		Constant = 2,
		Number = 3,				// not sure
		AnotherNumber = 4		// ???
	};

	enum class ResourceMarshallType : int
	{
		Unknown = 0,
		ResourceIndex = 11		// Access the network resource at specified index
	};

	enum class ResourceType : int
	{
		Unknown = 0,
		Resource = 4,
		Character = 5,			// i.e. genius, minions, agents, henchmen
		Entity = 23,
		Object = 24,
		Number = 26,
		Timer = 27,
		Item = 6634,			// i.e. weapons, briefcases
		GUIControl = 7682,
		EntityGroup = 8927,
		CharacterGroup = 8949
	};

	// Triple for storing parameters
	class Parameter
	{
	public:

		std::string m_type;
		std::int32_t m_defaultValue;
		std::string m_desc;

		Parameter(std::string type, std::int32_t defaultValue, std::string desc);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<Parameter> &params);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<Parameter> &params);
	};

	// Triple for resources
	class Resource
	{
	public:

		ResourceType m_type;
		std::string m_desc;
		std::int32_t m_defaultValue;

		Resource(ResourceType type, std::string desc, std::int32_t defaultValue);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<Resource> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<Resource> &resources);
	};
}
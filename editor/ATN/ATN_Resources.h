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
		Mysterious = 1,
		Constant = 2,			// This number is what will be sent in the marshall
		ParameterIndex = 3,		// Access the network parameter value at specified index
		ParameterNumber = 4		// Parameter value is distinguished from other numbers...?
	};

	enum class ResourceMarshallType : int
	{
		Unknown = 0,
		ResourceIndex = 1,				// Generic resource index (used for not null checks)
		ResourceIndexEntity = 2,		// Resource index of type Entity
		ResourceIndexTimer = 6,			// Resource index of type Timer
		ResourceIndexNumer = 11,		// Resource index of type Number
		ResourceIndexCharacter = 12,	// Resource index of type Character
		ResourceIndexObject = 13,		// Resource index of type Object
		ResourceIndexItem = 15,			// Resource index of type Item
		Number = 20						// N/A
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

	// Triple for storing parameter marshalls
	class ParameterMarshall
	{
	public:

		ParameterMarshallType m_type;
		std::int64_t m_value;

		ParameterMarshall(ParameterMarshallType type, std::int64_t value);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<ParameterMarshall> &params);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<ParameterMarshall> &params);
	};

	// Triple for resource marshalls
	class ResourceMarshall
	{
	public:

		ResourceMarshallType m_type;
		std::int64_t m_value;

		ResourceMarshall(ResourceMarshallType type, std::int64_t value);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<ResourceMarshall> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<ResourceMarshall> &resources);
	};

	// Triple for storing parameters
	class Parameter
	{
	public:

		std::string m_type;
		std::int64_t m_defaultValue;
		std::string m_desc;

		Parameter(std::string type, std::int64_t defaultValue, std::string desc);

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
		std::int64_t m_defaultValue;

		Resource(ResourceType type, std::string desc, std::int64_t defaultValue);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<Resource> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<Resource> &resources);
	};
}
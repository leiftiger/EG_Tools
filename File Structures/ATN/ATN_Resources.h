#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

// Better enums for reflections
#include "enum.h"

// Some inputs appear to be null, sending this value instead
// Not to be confused with -1, as that seems different as well
constexpr unsigned long ATN_NULL_VALUE = 4294967295;
// When a parameter is unintialized, it is often 0, so we need to make this a valid translation
constexpr unsigned long ATN_UNDEF_VALUE = 0;

namespace ATN
{

	enum class ParameterMarshallType : int
	{
		Unknown = 0,
		Mysterious = 1,
		Constant = 2,			// This number is what will be sent in the marshall
		ParameterIndex = 3,		// Access the network parameter value at specified index
		SmallInt = 4			// Seen used for small numbers, especially booleans
	};

	enum class ResourceMarshallType : int
	{
		Unknown = 0,
		ResourceIndex = 1,				// Generic resource index (used for not null checks)
		ResourceIndexEntity = 2,		// Resource index of type Entity
		ResourceIndexTimer = 6,			// Resource index of type Timer
		ResourceIndexNumber = 11,		// Resource index of type Number
		ResourceIndexCharacter = 12,	// Resource index of type Character
		ResourceIndexObject = 13,		// Resource index of type Object
		ResourceIndexItem = 15,			// Resource index of type Item
		ResourceIndexAny = 20			// Resource index of any type (?)
	};

	BETTER_ENUM(ResourceType, int,
		Resource = 4,
		Character = 5,			// i.e. genius, minions, agents, henchmen
		Entity = 23,
		Object = 24,
		Number = 26,
		Timer = 27,
		Item = 6634,			// i.e. weapons, briefcases
		GUIControl = 7682,
		EntityGroup = 8927,
		CharacterGroup = 8949)

	// Triple for storing parameter marshalls
	class ParameterMarshall
	{
	public:

		ParameterMarshallType m_type;
		std::int64_t m_value;

		ParameterMarshall(ParameterMarshallType type, std::int64_t value);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<ParameterMarshall*> &params);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<ParameterMarshall*> &params);
	};

	// Triple for resource marshalls
	class ResourceMarshall
	{
	public:

		ResourceMarshallType m_type;
		std::int64_t m_value;

		ResourceMarshall(ResourceMarshallType type, std::int64_t value);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<ResourceMarshall*> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<ResourceMarshall*> &resources);
	};

	// Triple for storing parameters
	class Parameter
	{
	public:

		std::string m_type;
		std::int64_t m_defaultValue;
		std::string m_desc;

		Parameter(std::string type, std::int64_t defaultValue, std::string desc);

		std::string translateValue(std::int64_t value) const;
		std::int64_t translateName(const std::string &name) const;

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<Parameter*> &params);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<Parameter*> &params);
	};

	// Triple for resources
	class Resource
	{
	public:

		ResourceType m_type;
		std::string m_desc;
		bool m_optionalResource;

		Resource(ResourceType type, std::string desc, bool optionalResource);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<Resource*> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<Resource*> &resources);
	};
}
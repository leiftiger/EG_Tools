#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

// Better enums for reflections
#include "enum.h"

#include "ATN_Object.h"

// Some inputs appear to be null, sending this value instead
// Not to be confused with -1, as that seems different as well
constexpr unsigned long ATN_NULL_VALUE = 4294967295;
// When a parameter is unintialized, it is often 0, so we need to make this a valid translation
constexpr unsigned long ATN_UNDEF_VALUE = 0;
// However, it is also sometimes -1 (since some types use 0)
constexpr int ATN_UNDEF_VALUE2 = -1;

namespace ATN
{

	enum class ParameterMarshallType : int
	{
		Constant = 2,			// This number is what will be sent in the marshall
		ParameterIndex = 3,		// Access the network parameter value at specified index
		SmallInt = 4			// Seen used for small numbers, especially booleans
	};

	enum class ResourceMarshallType : int
	{
		ResourceIndex = 1,					// Generic resource index (used for not null checks)
		ResourceIndexEntity = 2,			// Resource index of type Entity
		ResourceIndexTimer = 6,				// Resource index of type Timer
		ResourceIndexNumber = 11,			// Resource index of type Number
		ResourceIndexCharacter = 12,		// Resource index of type Character
		ResourceIndexObject = 13,			// Resource index of type Object
		ResourceIndexItem = 15,				// Resource index of type Item
		ResourceIndexGUIControl = 19,		// Resource index of type GUI Control
		ResourceIndexEntityGroup = 20,		// Resource index of entity group
		ResourceIndexCharacterGroup = 21,	// Resource index of character group
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

	// Triple for storing parameters
	class Parameter
	{
	private:
		static bool isNumber(const std::string &str);

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
		bool m_internalResource;

		Resource(ResourceType type, std::string desc, bool internalResource);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<Resource*> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<Resource*> &resources);
	};

	// Triple for storing parameter marshalls
	class ParameterMarshall
	{
	public:

		ParameterMarshallType m_type;
		std::int64_t m_value;

		ParameterMarshall(ParameterMarshallType type, std::int64_t value);
		ParameterMarshall(const Parameter &parameter);

		static ParameterMarshallType toParameterMarshallType(const Parameter &parameter);

		// Reset this marshall to pass a constant value of 0, possibly due to a transition signature change
		void resetConstant();
		// Resets this marshall to pass a constant value of 0 if it's referring to this index
		void resetConstant(std::int64_t index);

		// Swaps the given indices if this marshall points to any of them
		void swapIndices(std::int64_t index1, std::int64_t index2);

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<ParameterMarshall*> &params);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<ParameterMarshall*> &params);
	};

	// Triple for resource marshalls
	class ResourceMarshall
	{
	public:

		static constexpr std::int64_t INVALID_POINTER = -1;

		ResourceMarshallType m_type;
		std::int64_t m_value;

		ResourceMarshall(ResourceMarshallType type, std::int64_t value);
		ResourceMarshall(const Resource &resource, std::int64_t value = INVALID_POINTER);

		static ResourceMarshallType toResourceMarshallType(const ResourceType &t);

		// Points this marshall to nothing (this value cannot be saved)
		void reset();
		// Points this marshall to nothing if it's referring to this index
		void reset(std::int64_t index);

		// Points this marshall to the first available matching resource,
		// or nothing if nothing matches
		void reset(const std::vector<Resource*> &resources);
		// If the marshall refers to this index, then point this marshall 
		// to the first available matching resource, or nothing if nothing matches
		void reset(const std::vector<Resource*> &resources, std::int64_t index);

		// Swaps the given indices if this marshall points to any of them
		void swapIndices(std::int64_t index1, std::int64_t index2);

		bool acceptsResourceType(const ResourceType &t) const;

		// Deserialize from ATN string
		friend std::istream &operator>>(std::istream &stream, std::vector<ResourceMarshall*> &resources);
		// Serialize to ATN string
		friend std::ostream &operator<<(std::ostream &stream, const std::vector<ResourceMarshall*> &resources);
	};
}
#include "ATN_Resources.h"

#include "ATN_Exception.h"

#include <regex>
#include <sstream>

namespace ATN
{
	Parameter::Parameter(std::string type, std::int64_t defaultValue, std::string desc) : m_type(type), m_defaultValue(defaultValue), m_desc(desc)
	{

	}

	/*

	All existing parameter types:

	3D Gui Element
	Act Of Infamy ID
	Agent Group Type
	Agent Operation Type
	Animation Attachment
	Animation Biped Type
	Animation Flags
	Animation
	Boolean Value
	Camera View ID
	Character Tag
	Dialog Type
	Effect return value
	Entity Class
	Entity Type
	Event
	Floating Graphic
	Game Feature
	Game Flag
	GUI Control
	Hotspot
	Integer
	Interaction Reason
	LockName
	Mouse Button
	Move Flag
	Network ID
	Objective ID
	Region Activity
	Research card status
	Room Type
	ScaleType
	Sound Category ID
	Sound ID
	Spawn Type
	SpecialEffect
	String ID
	Terminate Interaction Priority
	UI State
	Video ID
	World Map Region
	World Region
	
	*/

	const std::string &Parameter::translateValue(std::int64_t value) const
	{
		if (m_type == "Boolean Value")
		{
			if (value == 1)
				return "true";
			else
				return "false";
		}
		else if (m_type == "Character Tag")
		{
			// TODO: Confirm
			switch (value)
			{
			case 0:
				return "IGNORE";
			case 1:
				return "KILL";
			case 2:
				return "CAPTURE";
			case 3:
				return "WEAKEN";
			}
		}

		if (Manager::hasHashValues(m_type.c_str()))
		{
			return Manager::getHashValues(m_type.c_str()).find((std::uint32_t)value).name();
		}

		return std::to_string(value);
	}

	std::int64_t Parameter::translateName(const std::string &name) const
	{
		if (m_type == "Boolean Value")
		{
			if (name == "true")
				return 1;
			else
				return 0;
		}
		else if (m_type == "Character Tag")
		{
			if (name == "IGNORE")
				return 0;
			else if (name == "KILL")
				return 1;
			else if (name == "CAPTURE")
				return 2;
			else if (name == "WEAKEN")
				return 3;
		}

		if (Manager::hasHashValues(m_type.c_str()))
		{
			return (std::int64_t)Manager::getHashValues(m_type.c_str()).find(name).id();
		}

		return std::stoll(name);
	}

	Resource::Resource(ResourceType type, std::string desc, bool optionalResource) : m_type(type), m_desc(desc), m_optionalResource(optionalResource)
	{

	}

	ParameterMarshall::ParameterMarshall(ParameterMarshallType type, std::int64_t value) : m_type(type), m_value(value)
	{

	}

	ResourceMarshall::ResourceMarshall(ResourceMarshallType type, std::int64_t value) : m_type(type), m_value(value)
	{

	}

	std::istream &operator>>(std::istream &stream, std::vector<ParameterMarshall> &params)
	{
		std::string line;

		util::getline(stream, line);

		int numEntries = std::stoi(line.substr(strlen("ParameterMarshall=")))/2;

		util::getline(stream, line);

		if (numEntries > 0)
		{
			const std::regex regStr("(-?[0-9]+) (-?[0-9]+) ");

			std::smatch matches;

			std::string subLine = line;

			for (int i = 0; i < numEntries; i++)
			{
				if (!std::regex_search(subLine, matches, regStr))
				{
					throw Exception("Missing %d arguments in \"%s\"", numEntries - i, line);
				}
				else
				{
					params.push_back(ParameterMarshall((ParameterMarshallType)std::stoll(matches[1]), std::stoll(matches[2])));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<ParameterMarshall> &params)
	{
		stream << "ParameterMarshall=" << params.size()*2 << std::endl;

		stream << "{ ";

		for (const ParameterMarshall &param : params)
		{
			stream << (int)param.m_type << " " << param.m_value << " ";
		}

		stream << "}" << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, std::vector<ResourceMarshall> &resources)
	{
		std::string line;

		util::getline(stream, line);

		int numEntries = std::stoi(line.substr(strlen("ResourceMarshall=")))/2;

		util::getline(stream, line);

		if (numEntries > 0)
		{
			const std::regex regStr("(-?[0-9]+) (-?[0-9]+) ");

			std::smatch matches;

			std::string subLine = line;

			for (int i = 0; i < numEntries; i++)
			{
				if (!std::regex_search(subLine, matches, regStr))
				{
					throw Exception("Missing %d arguments in \"%s\"", numEntries - i, line);
				}
				else
				{
					resources.push_back(ResourceMarshall((ResourceMarshallType)std::stoll(matches[1]), std::stoll(matches[2])));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<ResourceMarshall> &resources)
	{
		stream << "ResourceMarshall=" << resources.size()*2 << std::endl;

		stream << "{ ";

		for (const ResourceMarshall &resource : resources)
		{
			stream << (int)resource.m_type << " " << resource.m_value << " ";
		}

		stream << "}" << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, std::vector<Parameter> &params)
	{
		std::string line;

		util::getline(stream, line);

		int numEntries = std::stoi(line.substr(strlen("Parameters=")));

		util::getline(stream, line);

		if (numEntries > 0)
		{
			const std::regex regStr("\\{ \"([^\"]+)\" (-?[0-9]+) \"([^\"]+)\" \\} ");

			std::smatch matches;

			std::string subLine = line;

			for (int i = 0; i < numEntries; i++)
			{
				if (!std::regex_search(subLine, matches, regStr))
				{
					throw Exception("Missing %d parameters in \"%s\"", numEntries - i, line);
				}
				else
				{
					params.push_back(Parameter(matches[1], std::stoll(matches[2]), matches[3]));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<Parameter> &params)
	{
		stream << "Parameters=" << params.size() << std::endl;

		stream << "{ ";

		for (const Parameter &param : params)
		{
			stream << "{ \"" << param.m_type << "\" " << param.m_defaultValue << " \"" << param.m_desc << "\" } ";
		}

		stream << "}" << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, std::vector<Resource> &resources)
	{
		std::string line;

		util::getline(stream, line);

		int numEntries = std::stoi(line.substr(strlen("Resources=")));

		util::getline(stream, line);

		if (numEntries > 0)
		{
			const std::regex regStr("\\{ (-?[0-9]+) \"([^\"]+)\" (-?[0-9]+) \\} ");

			std::smatch matches;

			std::string subLine = line;

			for (int i = 0; i < numEntries; i++)
			{
				if (!std::regex_search(subLine, matches, regStr))
				{
					throw Exception("Missing %d resources in \"%s\"", numEntries -i, line);
				}
				else
				{
					resources.push_back(Resource((ResourceType)std::stoi(matches[1]), matches[2], std::stoi(matches[3])));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<Resource> &resources)
	{
		stream << "Resources=" << resources.size() << std::endl;

		stream << "{ ";

		for (const Resource &resource : resources)
		{
			stream << "{ " << (int)resource.m_type << " \"" << resource.m_desc << "\" " << (int)resource.m_optionalResource << " } ";
		}

		stream << "}" << std::endl;

		return stream;
	}
}
#include "ATN_Resources.h"

#include "ATN_Exception.h"

#include <regex>
#include <sstream>

namespace ATN
{
	bool Parameter::isNumber(const std::string &str)
	{
		if (str.size() == 0)
			return false;

		// Must be either a minus sign or a digit in the beginning
		if (isdigit(str[0]) == 0 && str[0] != '-')
			return false;

		for (size_t i = 1; i < str.size(); i++)
		{
			if (isdigit(str[i]) == 0)
				return false;
		}

		return true;
	}

	Parameter::Parameter(std::string type, std::int64_t defaultValue, std::string desc) : m_type(type), m_defaultValue(defaultValue), m_desc(desc)
	{

	}

	std::string Parameter::translateValue(std::int64_t value) const
	{
		if (m_type == "Integer")
		{
			// For integers, it appears we save as an unsigned int but really convert it to a signed int
			return std::to_string((std::int32_t)(std::uint32_t)value);
		}
		else if (m_type == "Network ID")
		{
			if (value == ATN_UNDEF_VALUE)
				return "UNDEFINED";

			Network *net = (Network*)&ATN::Manager::findByID((std::uint32_t)value);

			return std::to_string(net->id()) + std::string(": ") + net->name();
		}

		if (value == ATN_NULL_VALUE)
			return "NULL";

		if (Manager::hasDefinitions(m_type))
		{
			try
			{
				return Manager::getDefinitions(m_type).find((int32_t)value).name();
			}
			catch (ATN::Exception e)
			{
				if (value == ATN_UNDEF_VALUE)
					return "UNDEFINED";

				// Sometimes undefined values are set to something random
				// but it's also possible that we're somehow missing a defined string
				return std::string("MISSING: ") + std::to_string((int32_t)value);
			}
		}

		return std::to_string(value);
	}

	std::int64_t Parameter::translateName(const std::string &name) const
	{
		if (m_type == "Integer")
		{
			return (std::uint32_t)(std::int32_t)std::stoi(name);
		}
		else if (m_type == "Network ID")
		{
			if (name == "UNDEFINED")
				return ATN_UNDEF_VALUE;

			return std::stoll(name.substr(0, name.find(":")));
		}

		if (name == "NULL")
			return ATN_NULL_VALUE;

		if (Manager::hasDefinitions(m_type))
		{
			if (name == "UNDEFINED")
				return ATN_UNDEF_VALUE;

			if (name.size() >= strlen("MISSING: ") && name.substr(0, strlen("MISSING: ")) == "MISSING: ")
				return std::stoll(name.substr(strlen("MISSING: "), name.length() - strlen("MISSING: ")));


			return (std::int64_t)Manager::getDefinitions(m_type).find(name).id();
		}

		if (!isNumber(name))
			throw ATN::Exception("Missing definition for \"%s\", but didn't receive number!", m_type);

		return std::stoll(name);
	}

	Resource::Resource(ResourceType type, std::string desc, bool optionalResource) : m_type(type), m_desc(desc), m_internalResource(optionalResource)
	{

	}

	ParameterMarshall::ParameterMarshall(ParameterMarshallType type, std::int64_t value) : m_type(type), m_value(value)
	{

	}

	void ParameterMarshall::resetConstant()
	{
		m_type = ParameterMarshallType::Constant;
		m_value = 0;
	}

	void ParameterMarshall::resetConstant(std::int64_t index)
	{
		if (m_type == ParameterMarshallType::ParameterIndex && m_value == index)
		{
			resetConstant();
		}
	}

	ResourceMarshall::ResourceMarshall(ResourceMarshallType type, std::int64_t value) : m_type(type), m_value(value)
	{

	}

	ResourceMarshall::ResourceMarshall(const Resource *resource, std::int64_t value) : m_value(value)
	{
		m_type = toResourceMarshallType(resource->m_type);
	}

	ResourceMarshallType ResourceMarshall::toResourceMarshallType(const ResourceType &t)
	{
		switch (t)
		{
		case ResourceType::Resource:
			return ResourceMarshallType::ResourceIndex;
		case ResourceType::Character:
			return ResourceMarshallType::ResourceIndexCharacter;
		case ResourceType::Entity:
			return ResourceMarshallType::ResourceIndexEntity;
		case ResourceType::Object:
			return ResourceMarshallType::ResourceIndexObject;
		case ResourceType::Number:
			return ResourceMarshallType::ResourceIndexNumber;
		case ResourceType::Timer:
			return ResourceMarshallType::ResourceIndexTimer;
		case ResourceType::Item:
			return ResourceMarshallType::ResourceIndexItem;
		case ResourceType::GUIControl:
			return ResourceMarshallType::ResourceIndexGUIControl;
		case ResourceType::EntityGroup:
			return ResourceMarshallType::ResourceIndexEntityGroup;
		case ResourceType::CharacterGroup:
			return ResourceMarshallType::ResourceIndexCharacterGroup;
		}

		throw Exception("Unknown resource type!");

		return ResourceMarshallType::ResourceIndex;
	}

	bool ResourceMarshall::acceptsResourceType(const ResourceType &t) const
	{
		const ATN::Object *e;

		try
		{
			e = (ATN::Object*)&ATN::Manager::findByID(t);
		}
		catch (ATN::Exception e)
		{
			throw ATN::Exception("Cannot find resource dependencies! Make sure ATNResources.ros is loaded!");
		}

		while (e != nullptr)
		{
			if (m_type == toResourceMarshallType(ResourceType::_from_integral(e->id())))
				return true;

			e = e->parent();
		}

		return false;
	}

	std::istream &operator>>(std::istream &stream, std::vector<ParameterMarshall*> &params)
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
					params.push_back(new ParameterMarshall((ParameterMarshallType)std::stoll(matches[1]), std::stoll(matches[2])));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<ParameterMarshall*> &params)
	{
		stream << "ParameterMarshall=" << params.size()*2 << std::endl;

		stream << "{ ";

		for (const ParameterMarshall *param : params)
		{
			stream << (int)param->m_type << " " << param->m_value << " ";
		}

		stream << "}" << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, std::vector<ResourceMarshall*> &resources)
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
					resources.push_back(new ResourceMarshall((ResourceMarshallType)std::stoll(matches[1]), std::stoll(matches[2])));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<ResourceMarshall*> &resources)
	{
		stream << "ResourceMarshall=" << resources.size()*2 << std::endl;

		stream << "{ ";

		for (const ResourceMarshall *resource : resources)
		{
			stream << (int)resource->m_type << " " << resource->m_value << " ";
		}

		stream << "}" << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, std::vector<Parameter*> &params)
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
					params.push_back(new Parameter(matches[1], std::stoll(matches[2]), matches[3]));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<Parameter*> &params)
	{
		stream << "Parameters=" << params.size() << std::endl;

		stream << "{ ";

		for (const Parameter *param : params)
		{
			stream << "{ \"" << param->m_type << "\" " << param->m_defaultValue << " \"" << param->m_desc << "\" } ";
		}

		stream << "}" << std::endl;

		return stream;
	}

	std::istream &operator>>(std::istream &stream, std::vector<Resource*> &resources)
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
					resources.push_back(new Resource(ResourceType::_from_integral(std::stoi(matches[1])), matches[2], std::stoi(matches[3])));
					subLine = matches.suffix();
				}
			}
		}

		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const std::vector<Resource*> &resources)
	{
		stream << "Resources=" << resources.size() << std::endl;

		stream << "{ ";

		for (const Resource *resource : resources)
		{
			stream << "{ " << (int)resource->m_type << " \"" << resource->m_desc << "\" " << (int)resource->m_internalResource << " } ";
		}

		stream << "}" << std::endl;

		return stream;
	}
}
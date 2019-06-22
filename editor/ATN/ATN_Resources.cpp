#include "ATN_Resources.h"

#include "ATN_Exception.h"

#include <regex>
#include <sstream>

namespace ATN
{
	Parameter::Parameter(std::string type, std::int64_t defaultValue, std::string desc) : m_type(type), m_defaultValue(defaultValue), m_desc(desc)
	{

	}

	Resource::Resource(ResourceType type, std::string desc, std::int64_t defaultValue) : m_type(type), m_desc(desc), m_defaultValue(defaultValue)
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
					resources.push_back(Resource((ResourceType)std::stoi(matches[1]), matches[2], std::stoll(matches[3])));
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
			stream << "{ " << (int)resource.m_type << " \"" << resource.m_desc << "\" " << resource.m_defaultValue << " } ";
		}

		stream << "}" << std::endl;

		return stream;
	}
}
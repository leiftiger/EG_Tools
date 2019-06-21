#include "ATN_Resources.h"

#include "ATN_Exception.h"

#include <regex>
#include <sstream>

namespace ATN
{
	Parameter::Parameter(std::string type, std::int32_t defaultValue, std::string desc) : m_type(type), m_defaultValue(defaultValue), m_desc(desc)
	{

	}

	Resource::Resource(ResourceType type, std::string desc, std::int32_t defaultValue) : m_type(type), m_desc(desc), m_defaultValue(defaultValue)
	{

	}

	std::istream &operator>>(std::istream &stream, std::vector<Parameter> &params)
	{
		std::string line;

		util::getline(stream, line);

		int numEntries = std::stoi(line.substr(strlen("Parameters=")));

		util::getline(stream, line);

		if (numEntries > 0)
		{
			const std::regex regStr("\\{ \"([^\"]+)\" ([0-9]+) \"([^\"]+)\" \\} ");

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
					params.push_back(Parameter(matches[1], std::stoi(matches[2]), matches[3]));
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

		stream << "}";

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
			const std::regex regStr("\\{ ([0-9]+) \"([^\"]+)\" ([0-9]+) \\} ");

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
			stream << "{ " << (int)resource.m_type << " \"" << resource.m_desc << "\" " << resource.m_defaultValue << " } ";
		}

		stream << "}";

		return stream;
	}
}
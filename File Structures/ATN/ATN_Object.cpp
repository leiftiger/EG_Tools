#include "ATN_Object.h"

namespace ATN
{
	const char * const Object::typeName() const
	{
		return "TATNObjectType";
	}

	void Object::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << "ContainerID=ATNData" << std::endl;

		stream << "Parent=" << (m_parent == nullptr ? 0 : m_parent->id()) << std::endl;

		stream << "Resources=0" << std::endl;
		stream << "{ }" << std::endl;
		stream << "Parameters=0" << std::endl;
		stream << "{ }" << std::endl;
	}

	void Object::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int parentID = std::stoi(line.substr(strlen("Parent=")));

		if (parentID != 0)
			m_parent = (State*)&Manager::findByID(parentID);

		util::getline(stream, line);

		if (line != "Resources=0")
			throw Exception("Expected \"Resources=0\", got \"%s\"", line);

		util::getline(stream, line);

		if (line != "{ }")
			throw Exception("Expected \"{ }\", got \"%s\"", line);

		util::getline(stream, line);

		if (line != "Parameters=0")
			throw Exception("Expected \"Parameters=0\", got \"%s\"", line);

		util::getline(stream, line);

		if (line != "{ }")
			throw Exception("Expected \"{ }\", got \"%s\"", line);
	}
}
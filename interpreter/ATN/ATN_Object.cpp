#include "ATN_Object.h"

namespace ATN
{
	const char * const Object::typeName() const
	{
		return "TATNObjectType";
	}

	void Object::applyChanges(const Entry &originalEntry, const Entry &changeEntry)
	{
		Entry::applyChanges(originalEntry, changeEntry);

		const Object &original = (Object&)originalEntry;
		const Object &change = (Object&)changeEntry;

		// Why anyone would do this is a wild guess, but it's here anyway
		if (original.parent() == nullptr && change.parent() != nullptr)
		{
			if (this->parent() == nullptr)
				this->m_parent = change.m_parent;
		}
		else if (original.parent() != nullptr && change.parent() == nullptr)
		{
			if (this->parent() != nullptr && this->parent()->id() == original.parent()->id())
				this->m_parent = nullptr;
		}
		else
		{
			if (this->parent() != nullptr && this->parent()->id() == original.parent()->id())
				this->m_parent = change.m_parent;
		}
	}

	const Object *Object::parent() const
	{
		return m_parent;
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
			m_parent = (Object*)&Manager::findByID(parentID);

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

	bool Object::equals(const Entry *otherEntry) const
	{
		if (!Entry::equals(otherEntry))
			return false;

		const Object *other = (Object*)otherEntry;

		if (this->parent() == nullptr && other->parent() != nullptr)
			return false;

		if (this->parent() != nullptr && other->parent() == nullptr)
			return false;

		return (this->parent()->id() == other->parent()->id());
	}
}
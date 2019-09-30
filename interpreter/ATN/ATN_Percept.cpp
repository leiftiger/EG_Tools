#include "ATN_Percept.h"

namespace ATN
{
	Percept::Percept(std::string &gameFunction) : m_gameFunction(gameFunction)
	{

	}

	Percept::~Percept()
	{
		for (Resource *r : m_resources)
		{
			delete r;
		}

		for (Parameter *r : m_parameters)
		{
			delete r;
		}
	}

	bool Percept::isEditable() const
	{
		return false;
	}

	const std::string &Percept::gameFunction() const
	{
		return m_gameFunction;
	}

	void Percept::setGameFunction(std::string &gameFunction)
	{
		m_gameFunction = gameFunction;
	}

	const char * const Percept::typeName() const
	{
		return gameFunction().c_str();
	}

	const std::vector<Parameter*> &Percept::parameters() const
	{
		return m_parameters;
	}

	const std::vector<Resource*> &Percept::resources() const
	{
		return m_resources;
	}

	void Percept::applyChanges(const Entry &original, const Entry &change, DeltaMemory &memory)
	{
		throw std::exception("Delta changes not supported for percepts");
	}

	void Percept::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << m_resources << m_parameters;

		stream << "Compounds=0" << std::endl;
		stream << "Graph=16" << std::endl;
		stream << m_graphJunk << std::endl;
	}

	void Percept::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		stream >> m_resources >> m_parameters;

		std::string line;

		util::getline(stream, line);

		if (line != "Compounds=0")
			throw Exception("Expected \"Compounds=0\", got \"%s\"", line);

		util::getline(stream, line);

		if (line != "Graph=16")
			throw Exception("Expected \"Graph=16\", got \"%s\"", line);

		util::getline(stream, m_graphJunk);
	}

	bool Percept::equals(const Entry *otherEntry) const
	{
		if (!Entry::equals(otherEntry))
			return false;

		const Percept *other = (Percept*)otherEntry;

		if (this->gameFunction() != other->gameFunction())
			return false;

		if (this->m_graphJunk != other->m_graphJunk)
			return false;

		if (this->parameters().size() != other->parameters().size())
			return false;

		if (this->resources().size() != other->resources().size())
			return false;

		for (size_t i = 0; i < this->parameters().size(); i++)
		{
			if (*this->parameters()[i] != *other->parameters()[i])
				return false;
		}

		for (size_t i = 0; i < this->resources().size(); i++)
		{
			if (*this->resources()[i] != *other->resources()[i])
				return false;
		}

		return true;
	}
}
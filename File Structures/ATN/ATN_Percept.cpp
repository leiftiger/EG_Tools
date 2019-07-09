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
}
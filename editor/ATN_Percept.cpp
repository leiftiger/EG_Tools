#include "ATN_Percept.h"

namespace ATN
{
	Percept::Percept(std::string &gameFunction) : m_gameFunction(gameFunction)
	{

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

	void Percept::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << m_resources << m_parameters;

		stream << "Compounds=0" << std::endl;
		stream << "Graph=16" << std::endl;
		stream << m_graphJunk;
	}

	void Percept::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		stream >> m_resources >> m_parameters;

		std::string line;

		util::getline(stream, line); // compounds, never used
		util::getline(stream, line); // "Graph=16"

		util::getline(stream, m_graphJunk);

		util::getline(stream, line); // blank line - we reached the end of this object
	}
}
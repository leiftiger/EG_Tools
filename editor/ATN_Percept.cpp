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
		return this->m_gameFunction;
	}

	void Percept::setGameFunction(std::string &gameFunction)
	{
		this->m_gameFunction = gameFunction;
	}

	const char * const Percept::typeName() const
	{
		return this->gameFunction().c_str();
	}

	void Percept::serialize(std::ostream &stream) const
	{

	}

	void Percept::deserialize(std::istream &stream)
	{
		std::string line;

		// PLACEHOLDER, read until end of object to skip this class for now
		while (util::getline(stream, line))
		{
			if (line == "")
				break;
		}
	}
}
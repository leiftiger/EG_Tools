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
	void Percept::serialize(std::ostream & stream) const
	{
	}
	void Percept::deserialize(std::istream & stream)
	{
	}
}
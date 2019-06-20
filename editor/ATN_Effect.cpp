#include "ATN_Effect.h"

namespace ATN
{
	Effect::Effect(std::string &gameFunction) : m_gameFunction(gameFunction)
	{

	}

	bool Effect::isEditable() const
	{
		return false;
	}

	const std::string &Effect::gameFunction() const
	{
		return this->m_gameFunction;
	}

	void Effect::setGameFunction(std::string &gameFunction)
	{
		this->m_gameFunction = gameFunction;
	}
	void Effect::serialize(std::ostream & stream) const
	{
	}
	void Effect::deserialize(std::istream & stream)
	{
	}
}
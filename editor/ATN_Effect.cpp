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
		return m_gameFunction;
	}

	void Effect::setGameFunction(std::string &gameFunction)
	{
		m_gameFunction = gameFunction;
	}

	const char * const Effect::typeName() const
	{
		return gameFunction().c_str();
	}

	void Effect::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << m_resources << m_parameters;

		// This always seems to be zero
		stream << "Compounds=0" << std::endl;

		// Signify end of object
		stream << std::endl;
	}

	void Effect::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		stream >> m_resources >> m_parameters;

		util::getline(stream, line); // compounds, never used...
		util::getline(stream, line); // blank line - we reached the end of this object
	}
}
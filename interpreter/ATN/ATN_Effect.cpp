#include "ATN_Effect.h"

#include "utils.h"

namespace ATN
{
	Effect::Effect(std::string &gameFunction) : m_gameFunction(gameFunction)
	{

	}

	Effect::~Effect()
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

	const std::vector<Parameter*> &Effect::parameters() const
	{
		return m_parameters;
	}

	const std::vector<Resource*> &Effect::resources() const
	{
		return m_resources;
	}

	void Effect::applyChanges(const Entry &original, const Entry &change, DeltaMemory &memory)
	{
		throw std::runtime_error("Delta changes not supported for effects");
	}

	void Effect::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << m_resources << m_parameters;

		// This always seems to be zero
		stream << "Compounds=0" << std::endl;
	}

	void Effect::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		stream >> m_resources >> m_parameters;

		util::getline(stream, line);

		if (line != "Compounds=0")
			throw Exception("Expected \"Compounds=0\", got \"%s\"", line);
	}

	bool Effect::equals(const Entry *otherEntry) const
	{
		if (!Entry::equals(otherEntry))
			return false;

		const Effect *other = (Effect*)otherEntry;

		if (this->gameFunction() != other->gameFunction())
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
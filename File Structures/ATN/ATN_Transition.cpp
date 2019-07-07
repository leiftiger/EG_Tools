#include "ATN_Transition.h"

namespace ATN
{
	Transition::~Transition()
	{
		for (ResourceMarshall *r : m_effectResources)
			delete r;
		for (ResourceMarshall *r : m_perceptResources)
			delete r;

		for (ParameterMarshall *p : m_effectParameters)
			delete p;
		for (ParameterMarshall *p : m_perceptParameters)
			delete p;
	}

	const char * const Transition::typeName() const
	{
		return "TATNStateTransition";
	}

	void Transition::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << "ContainerID=ATNData" << std::endl;
		stream << "Effect=" << (int)(m_effect == nullptr ? -1 : m_effect->id()) << std::endl;

		stream << m_effectResources << m_effectParameters;

		stream << "ContainerID=ATNData" << std::endl;
		stream << "Percept=" << m_percept->id() << std::endl;

		stream << m_perceptResources << m_perceptParameters;

		stream << "ContainerID=ATNData" << std::endl;
		stream << "State=" << m_state->id() << std::endl;
	}

	void Transition::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int effectID = std::stoi(line.substr(strlen("Effect=")));

		if (effectID != -1)
			m_effect = (Effect*)&Manager::findByID(effectID);

		stream >> m_effectResources >> m_effectParameters;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int perceptID = std::stoi(line.substr(strlen("Percept=")));

		m_percept = (Percept*)&Manager::findByID(perceptID);

		stream >> m_perceptResources >> m_perceptParameters;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int stateID = std::stoi(line.substr(strlen("State=")));

		m_state = (State*)&Manager::findByID(stateID);
	}
}
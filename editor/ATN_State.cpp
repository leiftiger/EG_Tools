#include "ATN_State.h"

namespace ATN
{
	const char * const State::typeName() const
	{
		return "TATNState";
	}

	void State::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		// Header for transitions
		stream << "ContainerID=ATNData" << std::endl;

		util::writeEntryIDs<Transition>(stream, "StateTransitions=", m_stateTransitions);

		stream << "NetworkThreshold=0.0001" << std::endl;

		// Header for network transition
		stream << "ContainerID=ATNData" << std::endl;

		stream << "NetworkTransition=" << (m_networkTransition == nullptr ? -1 : m_networkTransition->id()) << std::endl;

		stream << m_resourceMarshalls << m_parameterMarshalls;
	}

	void State::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		m_stateTransitions = util::parseEntryIDs<Transition>(stream, "StateTransitions=");

		util::getline(stream, line);

		if (line != "NetworkThreshold=0.0001")
			throw Exception("Expected \"NetworkThreshold=0.0001\", got \"%s\"", line);

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int networkID = std::stoi(line.substr(strlen("NetworkTransition=")));

		if (networkID != -1)
		{
			m_networkTransition = (Network*)&Manager::findByID(networkID);
		}

		stream >> m_resourceMarshalls >> m_parameterMarshalls;
	}
}
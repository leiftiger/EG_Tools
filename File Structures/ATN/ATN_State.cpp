#include "ATN_State.h"

namespace ATN
{
	State::~State()
	{
		for (ResourceMarshall *r : m_resourceMarshalls)
			delete r;
		for (ParameterMarshall *p : m_parameterMarshalls)
			delete p;
	}

	const char * const State::typeName() const
	{
		return "TATNState";
	}

	const Network *State::networkTransition() const
	{
		return m_networkTransition;
	}

	void State::setNetworkTransition(Network *net)
	{
		m_networkTransition = net;

		for (ResourceMarshall *r : m_resourceMarshalls)
			delete r;
		for (ParameterMarshall *p : m_parameterMarshalls)
			delete p;

		m_resourceMarshalls.clear();
		m_parameterMarshalls.clear();

		if (m_networkTransition != nullptr)
		{
			for (Resource *r : net->resources())
			{
				if (!r->m_internalResource)
					m_resourceMarshalls.push_back(new ResourceMarshall(r, -1));
			}

			for (Parameter *p : net->parameters())
			{
				m_parameterMarshalls.push_back(new ParameterMarshall(ParameterMarshallType::Constant, 0));
			}
		}
	}

	const std::vector<ResourceMarshall*>& State::resourceMarshalls() const
	{
		return m_resourceMarshalls;
	}

	const std::vector<ParameterMarshall*>& State::parameterMarshalls() const
	{
		return m_parameterMarshalls;
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

		stream << "NetworkTransition=" << (int)(m_networkTransition == nullptr ? -1 : m_networkTransition->id()) << std::endl;

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
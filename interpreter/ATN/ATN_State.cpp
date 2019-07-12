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

	const std::vector<Transition*>& State::transitions() const
	{
		return m_stateTransitions;
	}

	void State::add(Transition &t)
	{
		m_stateTransitions.push_back(&t);
	}

	void State::moveUp(Transition &t)
	{
		m_stateTransitions.insert(--remove(t), &t);
	}

	void State::moveDown(Transition &t)
	{
		m_stateTransitions.insert(++remove(t), &t);
	}

	std::vector<Transition*>::iterator State::remove(Transition &t)
	{
		for (std::vector<Transition*>::iterator it = m_stateTransitions.begin(); it != m_stateTransitions.end(); it++)
		{
			if (*it == &t)
			{
				return m_stateTransitions.erase(it);
			}
		}

		return m_stateTransitions.end();
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

		/*
		std::vector<Transition*> revTransitions;

		for (int i = (int)m_stateTransitions.size() - 1; i >= 0; i--)
		{
			revTransitions.push_back(m_stateTransitions[i]);
		}
		*/

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

		/*
		std::vector<Transition*> revTransitions = util::parseEntryIDs<Transition>(stream, "StateTransitions=");

		// It appears that the transitions are sorted according from 0 = [least priority] to n = [most priority]
		// So the list is reversed to keep all methods working the same way
		
		for (int i = (int)revTransitions.size() - 1; i >= 0; i--)
		{
			m_stateTransitions.push_back(revTransitions[i]);
		}
		*/

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
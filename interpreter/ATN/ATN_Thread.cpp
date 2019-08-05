#include "ATN_Thread.h"

namespace ATN
{
	const char * const Thread::typeName() const
	{
		return "TATNThread";
	}

	const Network &Thread::network() const
	{
		return *m_network;
	}

	void Thread::setNetwork(Network &network)
	{
		m_network = &network;
	}

	const State *Thread::state() const
	{
		return m_state;
	}

	void Thread::setState(State *state)
	{
		m_state = state;
	}

	void Thread::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << "ContainerID=ATNData" << std::endl;
		stream << "Network=" << m_network->id() << std::endl;

		if (m_state == nullptr)
			throw ATN::Exception("Thread \"%s\" in network \"%s\" is not connected to any state", name(), m_network->name());

		stream << "ContainerID=ATNData" << std::endl;
		stream << "State=" << m_state->id() << std::endl;
	}

	void Thread::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int networkID = std::stoi(line.substr(strlen("Network=")));

		m_network = (Network*)&Manager::findByID(networkID);

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		util::getline(stream, line);

		int stateID = std::stoi(line.substr(strlen("State=")));

		m_state = (State*)&Manager::findByID(stateID);
	}
}
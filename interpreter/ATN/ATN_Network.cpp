#include "ATN_Network.h"

namespace ATN
{
	Network::~Network()
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

	const char * const Network::typeName() const
	{
		return "TATNNetwork";
	}

	const std::vector<State*> &Network::states() const
	{
		return m_states;
	}

	void Network::add(State &state)
	{
		m_states.push_back(&state);
	}

	void Network::moveUp(State &state)
	{
		m_states.insert(--remove(state), &state);
	}

	void Network::moveDown(State &state)
	{
		m_states.insert(++remove(state), &state);
	}

	std::vector<State*>::iterator Network::remove(State &state)
	{
		for (std::vector<State*>::iterator it = m_states.begin(); it != m_states.end(); it++)
		{
			if (*it == &state)
			{
				return m_states.erase(it);
			}
		}

		return m_states.end();
	}

	const std::vector<Thread*>& Network::threads() const
	{
		return m_threads;
	}

	void Network::add(Thread &thread)
	{
		m_threads.push_back(&thread);
	}

	void Network::moveUp(Thread &thread)
	{
		m_threads.insert(--remove(thread), &thread);
	}

	void Network::moveDown(Thread &thread)
	{
		m_threads.insert(++remove(thread), &thread);
	}

	std::vector<Thread*>::iterator Network::remove(Thread &thread)
	{
		for (std::vector<Thread*>::iterator it = m_threads.begin(); it != m_threads.end(); it++)
		{
			if (*it == &thread)
			{
				return m_threads.erase(it);
			}
		}

		return m_threads.end();
	}

	const std::vector<Resource*>& Network::resources() const
	{
		return m_resources;
	}

	void Network::add(Resource &resource)
	{
		m_resources.push_back(&resource);
	}

	void Network::moveUp(Resource &resource)
	{
		m_resources.insert(--remove(resource), &resource);
	}

	void Network::moveDown(Resource &resource)
	{
		m_resources.insert(++remove(resource), &resource);
	}

	std::vector<Resource*>::iterator Network::remove(Resource &resource)
	{
		for (std::vector<Resource*>::iterator it = m_resources.begin(); it != m_resources.end(); it++)
		{
			if (*it == &resource)
			{
				return m_resources.erase(it);
			}
		}

		return m_resources.end();
	}

	const std::vector<Parameter*>& Network::parameters() const
	{
		return m_parameters;
	}

	void Network::add(Parameter &param)
	{
		m_parameters.push_back(&param);
	}

	void Network::moveUp(Parameter &param)
	{
		std::vector<Parameter*>::iterator it = m_parameters.insert(--remove(param), &param);

		swapParameterMarshallIndices(it - m_parameters.begin(), (it + 1) - m_parameters.begin());
	}

	void Network::moveDown(Parameter &param)
	{
		std::vector<Parameter*>::iterator it = m_parameters.insert(++remove(param), &param);

		swapParameterMarshallIndices(it - m_parameters.begin(), (it - 1) - m_parameters.begin());
	}

	std::vector<Parameter*>::iterator Network::remove(Parameter &param)
	{
		for (std::vector<Parameter*>::iterator it = m_parameters.begin(); it != m_parameters.end(); it++)
		{
			if (*it == &param)
			{
				return m_parameters.erase(it);
			}
		}

		return m_parameters.end();
	}

	void Network::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << m_resources << m_parameters;

		// Header for states
		stream << "ContainerID=ATNData" << std::endl;

		util::writeEntryIDs<State>(stream, "States=", m_states);

		// Header for threads
		stream << "ContainerID=ATNData" << std::endl;

		util::writeEntryIDs<Thread>(stream, "Threads=", m_threads);
	}

	void Network::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		stream >> m_resources >> m_parameters;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		m_states = util::parseEntryIDs<State>(stream, "States=");

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		m_threads = util::parseEntryIDs<Thread>(stream, "Threads=");
	}

	void Network::swapParameterMarshallIndices(std::int64_t index1, std::int64_t index2)
	{
		for (State *state : m_states)
		{
			for (ParameterMarshall *paramMarshall : state->parameterMarshalls())
				paramMarshall->swapIndices(index1, index2);

			for (Transition *transition : state->transitions())
			{
				for (ParameterMarshall *paramMarshall : transition->effectParameterMarshalls())
					paramMarshall->swapIndices(index1, index2);

				for (ParameterMarshall *paramMarshall : transition->perceptParameterMarshalls())
					paramMarshall->swapIndices(index1, index2);
			}
		}

		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					// Here we swap the arguments that are passed to our network
					ParameterMarshall *temp = state->m_parameterMarshalls[index2];
					
					state->m_parameterMarshalls[index2] = state->m_parameterMarshalls[index1];
					state->m_parameterMarshalls[index1] = temp;
				}
			}
		}
	}
}
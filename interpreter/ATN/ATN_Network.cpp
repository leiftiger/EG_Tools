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

		if (!resource.m_internalResource)
			addResourceMarshall();
	}

	void Network::moveUp(Resource &resource)
	{
		m_moving = true;

		std::vector<Resource*>::iterator it = m_resources.insert(--remove(resource), &resource);

		swapResourceMarshallIndices(it - m_resources.begin(), (it + 1) - m_resources.begin());

		m_moving = false;
	}

	void Network::moveDown(Resource &resource)
	{
		m_moving = true;

		std::vector<Resource*>::iterator it = m_resources.insert(++remove(resource), &resource);

		swapResourceMarshallIndices(it - m_resources.begin(), (it - 1) - m_resources.begin());

		m_moving = false;
	}

	std::vector<Resource*>::iterator Network::remove(Resource &resource)
	{
		for (std::vector<Resource*>::iterator it = m_resources.begin(); it != m_resources.end(); it++)
		{
			if (*it == &resource)
			{
				if (!m_moving && !resource.m_internalResource)
					removeResourceMarshalls(it - m_resources.begin(), resource);

				return m_resources.erase(it);
			}
		}

		return m_resources.end();
	}

	void Network::updateResourceMarshalls(const Resource &resource)
	{
		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			for (ATN::State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					std::int64_t transitionIndex = -1;

					// Because the transitions only refer to the input indices, we have to take care to remove that index instead
					for (ATN::Resource *r : this->resources())
					{
						if (!r->m_internalResource)
							transitionIndex++;

						if (r == &resource)
							break;
					}

					if (!state->resourceMarshalls()[transitionIndex]->acceptsResourceType(resource.m_type))
					{
						state->resourceMarshalls()[transitionIndex]->m_type = ResourceMarshall::toResourceMarshallType(resource.m_type);
					}
				}
			}
		}
	}

	void Network::resetResourceMarshalls(std::int64_t index, const Resource &resource)
	{
		for (ATN::State *state : this->states())
		{
			for (ATN::ResourceMarshall *resourceMarshall : state->resourceMarshalls())
			{
				// We reset the marshall if the new type is incompatible to the current expected marshall
				if (resourceMarshall->m_value == index && !resourceMarshall->acceptsResourceType(resource.m_type))
					resourceMarshall->reset(this->resources());
			}

			for (ATN::Transition *transition : state->transitions())
			{
				for (ATN::ResourceMarshall *resourceMarshall : transition->effectResourceMarshalls())
				{
					if (resourceMarshall->m_value == index && !resourceMarshall->acceptsResourceType(resource.m_type))
						resourceMarshall->reset(this->resources());
				}
				for (ATN::ResourceMarshall *resourceMarshall : transition->perceptResourceMarshalls())
				{
					if (resourceMarshall->m_value == index && !resourceMarshall->acceptsResourceType(resource.m_type))
						resourceMarshall->reset(this->resources());
				}
			}
		}

		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			for (ATN::State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					std::int64_t transitionIndex = -1;

					// Because the transitions only refer to the input indices, we have to take care to remove that index instead
					for (ATN::Resource *r : this->resources())
					{
						if (!r->m_internalResource)
							transitionIndex++;

						if (r == &resource)
							break;
					}

					if (!state->resourceMarshalls()[transitionIndex]->acceptsResourceType(resource.m_type))
					{
						state->resourceMarshalls()[transitionIndex]->reset(net->resources());
					}
				}
			}
		}
	}

	void Network::resetInvalidResourceMarshalls()
	{
		for (ATN::State *state : this->states())
		{
			for (ATN::ResourceMarshall *resourceMarshall : state->resourceMarshalls())
			{
				if (resourceMarshall->m_value == ResourceMarshall::INVALID_POINTER)
					resourceMarshall->reset(this->resources());
			}

			for (ATN::Transition *transition : state->transitions())
			{
				for (ATN::ResourceMarshall *resourceMarshall : transition->effectResourceMarshalls())
				{
					if (resourceMarshall->m_value == ResourceMarshall::INVALID_POINTER)
						resourceMarshall->reset(this->resources());
				}
				for (ATN::ResourceMarshall *resourceMarshall : transition->perceptResourceMarshalls())
				{
					if (resourceMarshall->m_value == ResourceMarshall::INVALID_POINTER)
						resourceMarshall->reset(this->resources());
				}
			}
		}

		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			for (ATN::State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					std::int64_t transitionIndex = -1;

					// Because the transitions only refer to the input indices, we have to take care to remove that index instead
					for (ATN::Resource *r : this->resources())
					{
						if (!r->m_internalResource)
							transitionIndex++;

						if (state->resourceMarshalls()[transitionIndex]->m_value == ResourceMarshall::INVALID_POINTER)
							state->resourceMarshalls()[transitionIndex]->reset(net->resources());
					}
				}
			}
		}
	}

	void Network::setResourceInternal(Resource &resource, bool internal)
	{
		std::int64_t transitionIndex = -1;

		for (ATN::Resource *r : this->resources())
		{
			if (!r->m_internalResource)
				transitionIndex++;

			if (r == &resource)
			{
				if (!resource.m_internalResource)
					transitionIndex++;

				break;
			}
		}

		resource.m_internalResource = internal;

		if (internal)
		{
			for (ATN::Network *net : ATN::Manager::getNetworks())
			{
				for (ATN::State *state : net->states())
				{
					if (state->networkTransition() != nullptr && state->networkTransition() == this)
					{
						for (size_t i = transitionIndex; i < state->resourceMarshalls().size() - 1; i++)
						{
							state->m_resourceMarshalls[i] = state->m_resourceMarshalls[i + 1];
						}

						state->m_resourceMarshalls.pop_back();
					}
				}
			}
		}
		else
		{
			for (ATN::Network *net : ATN::Manager::getNetworks())
			{
				for (ATN::State *state : net->states())
				{
					if (state->networkTransition() != nullptr && state->networkTransition() == this)
					{
						ResourceMarshall *resourceMarshall = new ResourceMarshall(resource);

						resourceMarshall->reset(net->resources());

						state->m_resourceMarshalls.push_back(resourceMarshall);

						for (size_t i = state->m_resourceMarshalls.size()-1; i >= transitionIndex; i--)
						{
							state->m_resourceMarshalls[i + 1] = state->m_resourceMarshalls[i];
						}

						state->m_resourceMarshalls[transitionIndex] = resourceMarshall;
					}
				}
			}
		}
	}

	const std::vector<Parameter*>& Network::parameters() const
	{
		return m_parameters;
	}

	void Network::add(Parameter &param)
	{
		m_parameters.push_back(&param);

		addParameterMarshall();
	}

	void Network::moveUp(Parameter &param)
	{
		m_moving = true;

		std::vector<Parameter*>::iterator it = m_parameters.insert(--remove(param), &param);

		swapParameterMarshallIndices(it - m_parameters.begin(), (it + 1) - m_parameters.begin());

		m_moving = false;
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
				if (!m_moving)
					removeParameterMarshalls(it - m_parameters.begin());

				return m_parameters.erase(it);
			}
		}

		return m_parameters.end();
	}

	void Network::resetParameterMarshalls(std::int64_t index)
	{
		for (ATN::State *state : this->states())
		{
			for (ATN::ParameterMarshall *paramMarshall : state->parameterMarshalls())
			{
				// Any arguments that pointed to this index are reset to constant 0
				// so that if the variable is now incompatible with that argument, it won't create a broken state
				// (user has to find all uses themselves)
				paramMarshall->resetConstant(index);
			}

			for (ATN::Transition *transition : state->transitions())
			{
				for (ATN::ParameterMarshall *paramMarshall : transition->effectParameterMarshalls())
				{
					paramMarshall->resetConstant(index);
				}
				for (ATN::ParameterMarshall *paramMarshall : transition->perceptParameterMarshalls())
				{
					paramMarshall->resetConstant(index);
				}
			}
		}

		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			for (ATN::State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					// Here we reset the argument that will be passed to our index
					state->parameterMarshalls()[index]->resetConstant();
				}
			}
		}
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

	void Network::addParameterMarshall()
	{
		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					state->m_parameterMarshalls.push_back(new ParameterMarshall(*m_parameters[m_parameters.size()-1]));
				}
			}
		}
	}

	void Network::removeParameterMarshalls(std::int64_t index)
	{
		for (State *state : m_states)
		{
			for (ParameterMarshall *paramMarshall : state->parameterMarshalls())
				paramMarshall->resetConstant(index);

			for (Transition *transition : state->transitions())
			{
				for (ParameterMarshall *paramMarshall : transition->effectParameterMarshalls())
					paramMarshall->resetConstant(index);

				for (ParameterMarshall *paramMarshall : transition->perceptParameterMarshalls())
					paramMarshall->resetConstant(index);
			}
		}

		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					for (size_t i = index; i < state->parameterMarshalls().size() - 1; i++)
					{
						state->m_parameterMarshalls[i] = state->m_parameterMarshalls[i + 1];
					}

					state->m_parameterMarshalls.pop_back();
				}
			}
		}
	}
	void Network::swapResourceMarshallIndices(std::int64_t index1, std::int64_t index2)
	{
		for (State *state : m_states)
		{
			for (ResourceMarshall *resourceMarshall : state->resourceMarshalls())
				resourceMarshall->swapIndices(index1, index2);

			for (Transition *transition : state->transitions())
			{
				for (ResourceMarshall *resourceMarshall : transition->effectResourceMarshalls())
					resourceMarshall->swapIndices(index1, index2);

				for (ResourceMarshall *resourceMarshall : transition->perceptResourceMarshalls())
					resourceMarshall->swapIndices(index1, index2);
			}
		}

		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					// Here we swap the arguments that are passed to our network
					ResourceMarshall *temp = state->m_resourceMarshalls[index2];

					state->m_resourceMarshalls[index2] = state->m_resourceMarshalls[index1];
					state->m_resourceMarshalls[index1] = temp;
				}
			}
		}
	}

	void Network::addResourceMarshall()
	{
		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					state->m_resourceMarshalls.push_back(new ResourceMarshall(*m_resources[m_resources.size() - 1]));
				}
			}
		}
	}

	void Network::removeResourceMarshalls(std::int64_t index, const Resource &resource)
	{
		for (State *state : m_states)
		{
			for (ResourceMarshall *resourceMarshall : state->resourceMarshalls())
			{
				resourceMarshall->reset(resources(), index);
			}

			for (Transition *transition : state->transitions())
			{
				for (ResourceMarshall *resourceMarshall : transition->effectResourceMarshalls())
					resourceMarshall->reset(resources(), index);

				for (ResourceMarshall *resourceMarshall : transition->perceptResourceMarshalls())
					resourceMarshall->reset(resources(), index);
			}
		}

		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() != nullptr && state->networkTransition() == this)
				{
					std::int64_t transitionIndex = -1;

					// Because the transitions only refer to the input indices, we have to take care to remove that index instead
					for (Resource *r : m_resources)
					{
						if (!r->m_internalResource)
							transitionIndex++;

						if (r == &resource)
							break;
					}

					for (size_t i = transitionIndex; i < state->resourceMarshalls().size() - 1; i++)
					{
						state->m_resourceMarshalls[i] = state->m_resourceMarshalls[i + 1];
					}

					state->m_resourceMarshalls.pop_back();
				}
			}
		}
	}
}
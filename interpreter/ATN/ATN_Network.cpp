#include "ATN_Network.h"

#include "ATN_Manager.h"
#include "utils.h"

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

	void Network::applyChanges(const Entry &originalEntry, const Entry &changeEntry, DeltaMemory &memory)
	{
		Entry::applyChanges(originalEntry, changeEntry, memory);

		const Network &original = (Network&)originalEntry;
		const Network &change = (Network&)changeEntry;

		// Thread order is not important, just need to see if there were new additions or removals

		// Add new threads
		for (Thread *changeThread : change.threads())
		{
			bool bExists = false;

			for (Thread *originalThread : original.threads())
			{
				if (originalThread->id() == changeThread->id())
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				this->add(*changeThread);
			}
		}

		// Remove existing threads
		for (Thread *originalThread : original.threads())
		{
			bool bExists = false;

			for (Thread *changeThread : change.threads())
			{
				if (originalThread->id() == changeThread->id())
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				for (Thread *thisThread : this->threads())
				{
					if (thisThread->id() == originalThread->id())
					{
						this->remove(*thisThread);
						break;
					}
				}
			}
		}

		// Order-preserving delta updates
		deltaUpdateMemory(original.resources(), change.resources(), this->m_resources, memory, "resources");
		deltaUpdateMemory(original.parameters(), change.parameters(), this->m_parameters, memory, "parameters");

		// For states, the order is not important, so we only add or remove states like threads

		// Add new states
		for (State *changeState : change.states())
		{
			bool bExists = false;

			for (State *originalState : original.states())
			{
				if (originalState->id() == changeState->id())
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				this->add(*changeState);
			}
		}

		// Remove existing states
		for (State *originalState : original.states())
		{
			bool bExists = false;

			for (State *changeState : change.states())
			{
				if (originalState->id() == changeState->id())
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				for (State *thisState : this->states())
				{
					if (thisState->id() == originalState->id())
					{
						this->remove(*thisState);
						break;
					}
				}
			}
		}
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

		// We only need to swap input indices if both swapped resources were inputs
		if (!(*it)->m_internalResource && !(*(it + 1))->m_internalResource)
			swapResourceMarshallInputs(**it);

		m_moving = false;
	}

	void Network::moveDown(Resource &resource)
	{
		m_moving = true;

		std::vector<Resource*>::iterator it = m_resources.insert(++remove(resource), &resource);

		swapResourceMarshallIndices(it - m_resources.begin(), (it - 1) - m_resources.begin());

		// We only need to swap input indices if both swapped resources were inputs
		if (!(*it)->m_internalResource && !(*(it - 1))->m_internalResource)
			swapResourceMarshallInputs(**(it - 1));

		m_moving = false;
	}

	std::vector<Resource*>::iterator Network::remove(Resource &resource)
	{
		for (std::vector<Resource*>::iterator it = m_resources.begin(); it != m_resources.end(); it++)
		{
			if (*it == &resource)
			{
				std::int64_t itIndex = it - m_resources.begin();

				std::vector<Resource*>::iterator itRem = m_resources.erase(it);

				if (!m_moving)
					removeResourceMarshalls(itIndex, resource);

				return itRem;
			}
		}

		return m_resources.end();
	}

	void Network::updateResourceMarshalls(const Resource &resource)
	{
		if (!resource.m_internalResource)
		{
			for (ATN::Network *net : ATN::Manager::getNetworks())
			{
				for (ATN::State *state : net->states())
				{
					if (state->networkTransition() == this)
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

		if (!resource.m_internalResource)
		{
			for (ATN::Network *net : ATN::Manager::getNetworks())
			{
				for (ATN::State *state : net->states())
				{
					if (state->networkTransition() == this)
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
						{
							transitionIndex++;

							if (state->resourceMarshalls()[transitionIndex]->m_value == ResourceMarshall::INVALID_POINTER)
								state->resourceMarshalls()[transitionIndex]->reset(net->resources());
						}
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
				if (resource.m_internalResource)
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
					if (state->networkTransition() == this)
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
					if (state->networkTransition() == this)
					{
						ResourceMarshall *resourceMarshall = new ResourceMarshall(resource);

						resourceMarshall->reset(net->resources());

						state->m_resourceMarshalls.push_back(resourceMarshall);

						for (int i = (int)state->m_resourceMarshalls.size() - 2; i >= transitionIndex; i--)
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
		m_moving = true;

		std::vector<Parameter*>::iterator it = m_parameters.insert(++remove(param), &param);

		swapParameterMarshallIndices(it - m_parameters.begin(), (it - 1) - m_parameters.begin());

		m_moving = false;
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
				// so that if the parameter is now incompatible with that argument, it won't create a broken state
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
				if (state->networkTransition() == this)
				{
					// Here we reset the argument that will be passed to our index
					state->parameterMarshalls()[index]->resetConstant();
				}
			}
		}
	}

	bool Network::hasResourceInputs() const
	{
		for (Resource *r : m_resources)
			if (!r->m_internalResource)
				return true;

		return false;
	}

	int Network::countDependencies(Resource &resource, bool includeInternal, bool includeExternal) const
	{
		int numDependencies = 0;

		if (includeInternal)
		{
			std::int64_t index = -1;

			for (size_t i = 0; i < m_resources.size(); i++)
			{
				if (m_resources[i] == &resource)
				{
					index = (std::int64_t)i;
					break;
				}
			}

			for (ATN::State *state : this->states())
			{
				for (ATN::ResourceMarshall *resourceMarshall : state->resourceMarshalls())
				{
					if (resourceMarshall->m_value == index)
						numDependencies++;
				}

				for (ATN::Transition *transition : state->transitions())
				{
					for (ATN::ResourceMarshall *resourceMarshall : transition->effectResourceMarshalls())
					{
						if (resourceMarshall->m_value == index)
							numDependencies++;
					}
					for (ATN::ResourceMarshall *resourceMarshall : transition->perceptResourceMarshalls())
					{
						if (resourceMarshall->m_value == index)
							numDependencies++;
					}
				}
			}
		}

		if (includeExternal)
		{
			if (!resource.m_internalResource)
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
								{
									transitionIndex++;

									if (r == &resource)
									{
										numDependencies++;
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		return numDependencies;
	}

	int Network::countDependencies(Parameter &param, bool includeInternal, bool includeExternal) const
	{
		int numDependencies = 0;

		if (includeInternal)
		{
			std::int64_t index = -1;

			for (size_t i = 0; i < m_parameters.size(); i++)
			{
				if (m_parameters[i] == &param)
				{
					index = (std::int64_t)i;
					break;
				}
			}

			for (ATN::State *state : this->states())
			{
				for (ATN::ParameterMarshall *paramMarshall : state->parameterMarshalls())
				{
					if (paramMarshall->m_type == ParameterMarshallType::ParameterIndex && paramMarshall->m_value == index)
						numDependencies++;
				}

				for (ATN::Transition *transition : state->transitions())
				{
					for (ATN::ParameterMarshall *paramMarshall : transition->effectParameterMarshalls())
					{
						if (paramMarshall->m_type == ParameterMarshallType::ParameterIndex && paramMarshall->m_value == index)
							numDependencies++;
					}

					for (ATN::ParameterMarshall *paramMarshall : transition->perceptParameterMarshalls())
					{
						if (paramMarshall->m_type == ParameterMarshallType::ParameterIndex && paramMarshall->m_value == index)
							numDependencies++;
					}
				}
			}
		}

		if (includeExternal)
		{
			for (ATN::Network *net : ATN::Manager::getNetworks())
			{
				for (ATN::State *state : net->states())
				{
					if (state->networkTransition() == this)
					{
						numDependencies++;
					}
				}
			}
		}

		return numDependencies;
	}

	void Network::validateMarshalls(Transition &transition) const
	{
		for (size_t i = 0; i < transition.effectParameterMarshalls().size(); i++)
		{
			ATN::ParameterMarshall *paramMarshall = transition.effectParameterMarshalls()[i];

			// Constants can be kept as they are
			if (paramMarshall->m_type == ParameterMarshallType::ParameterIndex)
			{
				// Points to non-existent parameter
				if (paramMarshall->m_value >= (std::int64_t)m_parameters.size())
					paramMarshall->resetConstant();
				else
				{
					// Parameter at this index can't be used for the transition
					if (m_parameters[paramMarshall->m_value]->m_type != transition.effect()->parameters()[i]->m_type)
						paramMarshall->resetConstant();
				}
			}
		}

		for (size_t i = 0; i < transition.perceptParameterMarshalls().size(); i++)
		{
			ATN::ParameterMarshall *paramMarshall = transition.perceptParameterMarshalls()[i];

			// Constants can be kept as they are
			if (paramMarshall->m_type == ParameterMarshallType::ParameterIndex)
			{
				// Points to non-existent parameter
				if (paramMarshall->m_value >= (std::int64_t)m_parameters.size())
					paramMarshall->resetConstant();
				else
				{
					// Parameter at this index can't be used for the transition
					if (m_parameters[paramMarshall->m_value]->m_type != transition.percept()->parameters()[i]->m_type)
						paramMarshall->resetConstant();
				}
			}
		}

		for (ATN::ResourceMarshall *resourceMarshall : transition.effectResourceMarshalls())
		{
			// Points to non-existent resource
			if (resourceMarshall->m_value >= (std::int64_t)m_resources.size())
				resourceMarshall->reset();
			else
			{
				// Resource at this index can't be used for the transition
				if (!resourceMarshall->acceptsResourceType(m_resources[resourceMarshall->m_value]->m_type))
					resourceMarshall->reset();
			}
		}

		for (ATN::ResourceMarshall *resourceMarshall : transition.perceptResourceMarshalls())
		{
			// Points to non-existent resource
			if (resourceMarshall->m_value >= (std::int64_t)m_resources.size())
				resourceMarshall->reset();
			else
			{
				// Resource at this index can't be used for the transition
				if (!resourceMarshall->acceptsResourceType(m_resources[resourceMarshall->m_value]->m_type))
					resourceMarshall->reset();
			}
		}
	}

	void Network::validateMarshalls(State &state) const
	{
		for (size_t i = 0; i < state.parameterMarshalls().size(); i++)
		{
			ATN::ParameterMarshall *paramMarshall = state.parameterMarshalls()[i];

			// Constants can be kept as they are
			if (paramMarshall->m_type == ParameterMarshallType::ParameterIndex)
			{
				// Points to non-existent parameter
				if (paramMarshall->m_value >= (std::int64_t)m_parameters.size())
					paramMarshall->resetConstant();
				else
				{
					// Parameter at this index can't be used for the transition
					if (m_parameters[paramMarshall->m_value]->m_type != state.networkTransition()->parameters()[i]->m_type)
						paramMarshall->resetConstant();
				}
			}
		}

		for (ATN::ResourceMarshall *resourceMarshall : state.resourceMarshalls())
		{
			// Points to non-existent resource
			if (resourceMarshall->m_value >= (std::int64_t)m_resources.size())
				resourceMarshall->reset();
			else
			{
				// Resource at this index can't be used for the transition
				if (!resourceMarshall->acceptsResourceType(m_resources[resourceMarshall->m_value]->m_type))
					resourceMarshall->reset();
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

	bool Network::equals(const Entry *otherEntry) const
	{
		if (!Entry::equals(otherEntry))
			return false;

		const Network *other = (Network*)otherEntry;

		if (this->states().size() != other->states().size())
			return false;

		if (this->threads().size() != other->threads().size())
			return false;

		if (this->parameters().size() != other->parameters().size())
			return false;

		if (this->resources().size() != other->resources().size())
			return false;

		for (size_t i = 0; i < this->states().size(); i++)
		{
			if (this->states()[i]->id() != other->states()[i]->id())
				return false;
		}

		for (size_t i = 0; i < this->threads().size(); i++)
		{
			if (this->threads()[i]->id() != other->threads()[i]->id())
				return false;
		}

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
				if (state->networkTransition() == this)
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
				if (state->networkTransition() == this)
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
				if (state->networkTransition() == this)
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
	}

	void Network::swapResourceMarshallInputs(const Resource &resource)
	{
		for (Network *net : ATN::Manager::getNetworks())
		{
			for (State *state : net->states())
			{
				if (state->networkTransition() == this)
				{
					std::int64_t transitionIndex = -1;

					for (Resource *r : m_resources)
					{
						if (!r->m_internalResource)
							transitionIndex++;

						if (r == &resource)
							break;
					}

					// Because this is only called with two input resources in a row, we should always expect to have an extra index here
					ResourceMarshall *temp = state->m_resourceMarshalls[transitionIndex];

					state->m_resourceMarshalls[transitionIndex] = state->m_resourceMarshalls[transitionIndex + 1];
					state->m_resourceMarshalls[transitionIndex + 1] = temp;
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
				if (state->networkTransition() == this)
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

		// If resource was just internal then no networks could've used it
		if (!resource.m_internalResource)
		{
			for (Network *net : ATN::Manager::getNetworks())
			{
				for (State *state : net->states())
				{
					if (state->networkTransition() == this)
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
}
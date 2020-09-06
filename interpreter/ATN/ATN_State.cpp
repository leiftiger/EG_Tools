#include "ATN_State.h"

#include "utils.h"

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

	void State::applyChanges(const Entry &originalEntry, const Entry &changeEntry, DeltaMemory &memory)
	{
		Entry::applyChanges(originalEntry, changeEntry, memory);

		const State &original = (State&)originalEntry;
		const State &change = (State&)changeEntry;

		// Added new network transition at this state
		if (original.networkTransition() == nullptr && change.networkTransition() != nullptr)
		{
			if (this->networkTransition() == nullptr)
			{
				m_networkTransition = change.networkTransition();

				for (ResourceMarshall *r : change.resourceMarshalls())
				{
					m_resourceMarshalls.push_back(new ResourceMarshall(*r));
				}

				for (ParameterMarshall *p : change.parameterMarshalls())
				{
					m_parameterMarshalls.push_back(new ParameterMarshall(*p));
				}

				memory.lock("resources");
				memory.lock("parameters");
			}
		}
		// Removed network transition at this state
		else if (original.networkTransition() != nullptr && change.networkTransition() == nullptr)
		{
			if (this->networkTransition() != nullptr && this->networkTransition()->id() == original.networkTransition()->id())
			{
				this->setNetworkTransition(nullptr);

				memory.lock("resources");
				memory.lock("parameters");
			}
		}
		else if (original.networkTransition() == nullptr && change.networkTransition() == nullptr)
		{
			// Do nothing
		}
		else
		{
			bool allowUpdate = true;
			bool lockAfter = false;

			// Might be problematic to change the transition, but hopefully the marshalls should capture the change as well
			if (original.networkTransition()->id() != change.networkTransition()->id())
			{
				if (this->networkTransition() != nullptr && this->networkTransition()->id() == original.networkTransition()->id())
				{
					// If some other mod has changed the marshalls, then we can't change the transition without breaking that
					if (memory.isChanged("resources") || memory.isChanged("parameters"))
					{
						allowUpdate = false;
						lockAfter = true;
					}
					else
					{
						this->m_networkTransition = change.networkTransition();

						// Prevent other mods from doing marshall updates as they would be dependent on the original transition
						lockAfter = true;
					}
				}
			}

			if (allowUpdate)
			{
				if (!memory.isLocked("resources"))
					deltaUpdateMemory(original.resourceMarshalls(), change.resourceMarshalls(), this->m_resourceMarshalls, memory, "resources");

				if (!memory.isLocked("parameters"))
					deltaUpdateMemory(original.parameterMarshalls(), change.parameterMarshalls(), this->m_parameterMarshalls, memory, "parameters");
			}

			if (lockAfter)
			{
				memory.lock("resources");
				memory.lock("parameters");
			}
		}

		// While the order for transitions is largely unimportant, we make sure we capture the changes by the mod either way
		if (!memory.isLocked("transitions"))
			deltaUpdateID(original.transitions(), change.transitions(), this->m_stateTransitions, memory, "transitions");
	}

	const std::vector<Transition*> &State::transitions() const
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

	Network *State::networkTransition() const
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
					m_resourceMarshalls.push_back(new ResourceMarshall(*r));
			}

			for (Parameter *p : net->parameters())
			{
				m_parameterMarshalls.push_back(new ParameterMarshall(*p));
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

	void State::copyNetworkTransition(const State *other)
	{
		this->setNetworkTransition(other->networkTransition());

		if (other->networkTransition() != nullptr)
		{
			for (size_t i = 0; i < other->parameterMarshalls().size(); i++)
			{
				this->parameterMarshalls()[i]->m_type = other->parameterMarshalls()[i]->m_type;
				this->parameterMarshalls()[i]->m_value = other->parameterMarshalls()[i]->m_value;
			}

			for (size_t i = 0; i < other->resourceMarshalls().size(); i++)
			{
				this->resourceMarshalls()[i]->m_type = other->resourceMarshalls()[i]->m_type;
				this->resourceMarshalls()[i]->m_value = other->resourceMarshalls()[i]->m_value;
			}
		}
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

	bool State::equals(const Entry *otherEntry) const
	{
		if (!Entry::equals(otherEntry))
			return false;

		const State *other = (State*)otherEntry;

		if (this->transitions().size() != other->transitions().size())
			return false;

		if (this->networkTransition() == nullptr && other->networkTransition() != nullptr)
			return false;

		if (this->networkTransition() != nullptr && other->networkTransition() == nullptr)
			return false;

		if (this->networkTransition() != nullptr && this->networkTransition()->id() != other->networkTransition()->id())
			return false;

		if (this->parameterMarshalls().size() != other->parameterMarshalls().size())
			return false;

		if (this->resourceMarshalls().size() != other->resourceMarshalls().size())
			return false;

		for (size_t i = 0; i < this->transitions().size(); i++)
		{
			if (this->transitions()[i]->id() != other->transitions()[i]->id())
				return false;
		}

		for (size_t i = 0; i < this->parameterMarshalls().size(); i++)
		{
			if (*this->parameterMarshalls()[i] != *other->parameterMarshalls()[i])
				return false;
		}

		for (size_t i = 0; i < this->resourceMarshalls().size(); i++)
		{
			if (*this->resourceMarshalls()[i] != *other->resourceMarshalls()[i])
				return false;
		}

		return true;
	}
}
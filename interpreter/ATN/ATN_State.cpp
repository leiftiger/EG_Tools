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

	void State::applyChanges(const Entry &originalEntry, const Entry &changeEntry)
	{
		Entry::applyChanges(originalEntry, changeEntry);

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
			}
		}
		// Removed network transition at this state
		else if (original.networkTransition() != nullptr && change.networkTransition() == nullptr)
		{
			if (this->networkTransition() != nullptr && this->networkTransition()->id() == original.networkTransition()->id())
			{
				this->setNetworkTransition(nullptr);
			}
		}
		else
		{
			// TODO: Could maybe be problematic to change the transition, but hopefully the marshalls should capture the change as well
			if (original.networkTransition()->id() != change.networkTransition()->id())
			{
				if (this->networkTransition() != nullptr && this->networkTransition()->id() == original.networkTransition()->id())
				{
					this->m_networkTransition = change.networkTransition();
				}
			}

			// Add new resource marshalls
			for (ResourceMarshall *changeResource : change.resourceMarshalls())
			{
				bool bExists = false;

				for (ResourceMarshall *originalResource : original.resourceMarshalls())
				{
					if (*originalResource == *changeResource)
					{
						bExists = true;
						break;
					}
				}

				if (!bExists)
				{
					m_resourceMarshalls.push_back(new ResourceMarshall(*changeResource));
				}
			}

			// Remove existing resource marshalls
			for (ResourceMarshall *originalResource : original.resourceMarshalls())
			{
				bool bExists = false;

				for (ResourceMarshall *changeResource : change.resourceMarshalls())
				{
					if (*originalResource == *changeResource)
					{
						bExists = true;
						break;
					}
				}

				if (!bExists)
				{
					for (std::vector<ResourceMarshall*>::iterator it = m_resourceMarshalls.begin(); it != m_resourceMarshalls.end(); it++)
					{
						ResourceMarshall *thisResource = *it;

						if (*thisResource == *originalResource)
						{
							m_resourceMarshalls.erase(it);

							delete thisResource;
							break;
						}
					}
				}
			}

			// Perform modification assuming both original and change were using the same indices
			// TODO: Try to improve this similar to Network::applyChanges()
			for (int i = 0; i < std::min(original.resourceMarshalls().size(), change.resourceMarshalls().size()); i++)
			{
				ResourceMarshall *originalResource = original.resourceMarshalls()[i];
				ResourceMarshall *changeResource = change.resourceMarshalls()[i];

				if (*originalResource != *changeResource)
				{
					for (int i2 = 0; i2 < this->resourceMarshalls().size(); i2++)
					{
						ResourceMarshall *thisResource = this->resourceMarshalls()[i2];

						// We only modify the resource if we can still find the original somewhere
						// (otherwise this means we've already modified this resource)
						if (*thisResource == *originalResource)
						{
							this->m_resourceMarshalls[i2] = new ResourceMarshall(*changeResource);

							delete thisResource;
							break;
						}
					}
				}
			}

			// Add new parameter marshalls
			for (ParameterMarshall *changeParam : change.parameterMarshalls())
			{
				bool bExists = false;

				for (ParameterMarshall *originalParam : original.parameterMarshalls())
				{
					if (*originalParam == *changeParam)
					{
						bExists = true;
						break;
					}
				}

				if (!bExists)
				{
					m_parameterMarshalls.push_back(new ParameterMarshall(*changeParam));
				}
			}

			// Remove existing parameter marshalls
			for (ParameterMarshall *originalParam : original.parameterMarshalls())
			{
				bool bExists = false;

				for (ParameterMarshall *changeParam : change.parameterMarshalls())
				{
					if (*originalParam == *changeParam)
					{
						bExists = true;
						break;
					}
				}

				if (!bExists)
				{
					for (std::vector<ParameterMarshall*>::iterator it = m_parameterMarshalls.begin(); it != m_parameterMarshalls.end(); it++)
					{
						ParameterMarshall *thisParam = *it;

						if (*thisParam == *originalParam)
						{
							m_parameterMarshalls.erase(it);

							delete thisParam;
							break;
						}
					}
				}
			}

			// Perform modification assuming both original and change were using the same indices
			// TODO: Try to improve this similar to Network::applyChanges()
			for (int i = 0; i < std::min(original.parameterMarshalls().size(), change.parameterMarshalls().size()); i++)
			{
				ParameterMarshall *originalParam = original.parameterMarshalls()[i];
				ParameterMarshall *changeParam = change.parameterMarshalls()[i];

				if (*originalParam != *changeParam)
				{
					for (int i2 = 0; i2 < this->parameterMarshalls().size(); i2++)
					{
						ParameterMarshall *thisParam = this->parameterMarshalls()[i2];

						// We only modify the resource if we can still find the original somewhere
						// (otherwise this means we've already modified this resource)
						if (*thisParam == *originalParam)
						{
							this->m_parameterMarshalls[i2] = new ParameterMarshall(*changeParam);

							delete thisParam;
							break;
						}
					}
				}
			}
		}

		// Add new transitions
		for (Transition *changeTransition : change.transitions())
		{
			bool bExists = false;

			for (Transition *originalTransition : original.transitions())
			{
				if (originalTransition->id() == changeTransition->id())
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				this->add(*changeTransition);
			}
		}

		// Remove existing transitions
		for (Transition *originalTransition : original.transitions())
		{
			bool bExists = false;

			for (Transition *changeTransition : change.transitions())
			{
				if (originalTransition->id() == changeTransition->id())
				{
					bExists = true;
					break;
				}
			}

			if (!bExists)
			{
				for (Transition *thisTransition : this->transitions())
				{
					if (thisTransition->id() == originalTransition->id())
					{
						this->remove(*thisTransition);
						break;
					}
				}
			}
		}

		// While the order won't matter for most transitions, it's possible that some mods depend on this,
		// but then we assume the same things as above and in Network::applyChanges
		for (int i = 0; i < std::min(original.transitions().size(), change.transitions().size()); i++)
		{
			Transition *originalTransition = original.transitions()[i];
			Transition *changeTransition = change.transitions()[i];

			if (originalTransition->id() != changeTransition->id())
			{
				for (int i2 = 0; i2 < this->transitions().size(); i2++)
				{
					Transition *thisTransition = this->transitions()[i2];

					// We only modify the transition if we can still find the original somewhere
					// (otherwise this means we've already modified this transition)
					if (thisTransition->id() == originalTransition->id())
					{
						this->m_stateTransitions[i2] = changeTransition;

						break;
					}
				}
			}
		}
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
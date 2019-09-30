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

	void Transition::applyChanges(const Entry &originalEntry, const Entry &changeEntry, DeltaMemory &memory)
	{
		Entry::applyChanges(originalEntry, changeEntry, memory);

		const Transition &original = (Transition&)originalEntry;
		const Transition &change = (Transition&)changeEntry;

		// This should probably be the most common change for transitions
		if (original.state()->id() != change.state()->id())
		{
			if (this->state()->id() == original.state()->id())
			{
				this->setState(change.m_state);
			}
		}

		// Unlike percepts, effects could be null
		if (original.effect() == nullptr && change.effect() != nullptr)
		{
			if (this->effect() == nullptr)
			{
				this->m_effect = change.effect();

				for (ResourceMarshall *r : change.effectResourceMarshalls())
				{
					m_effectResources.push_back(new ResourceMarshall(*r));
				}

				for (ParameterMarshall *p : change.effectParameterMarshalls())
				{
					m_effectParameters.push_back(new ParameterMarshall(*p));
				}

				memory.lock("effect_resources");
				memory.lock("effect_parameters");
			}
		}
		else if (original.effect() != nullptr && change.effect() == nullptr)
		{
			if (this->effect() != nullptr && this->effect()->id() == original.effect()->id())
			{
				this->setEffect(nullptr);

				memory.lock("effect_resources");
				memory.lock("effect_parameters");
			}
		}
		else if (original.effect() == nullptr && change.effect() == nullptr)
		{
			// Do nothing
		}
		else
		{
			bool allowUpdate = true;
			bool lockAfter = false;

			// Might be problematic to change the effect, but hopefully the marshalls should capture the change as well
			if (original.effect()->id() != change.effect()->id())
			{
				if (this->effect() != nullptr && this->effect()->id() == original.effect()->id())
				{
					// If some other mod has changed the marshalls, then we can't change the effect without breaking that
					if (memory.isChanged("effect_resources") || memory.isChanged("effect_parameters"))
					{
						allowUpdate = false;
						lockAfter = true;
					}
					else
					{
						m_effect = change.effect();

						// Prevent other mods from doing marshall updates as they would be dependent on the original transition
						lockAfter = true;
					}
				}
			}

			if (allowUpdate)
			{
				if (!memory.isLocked("effect_resources"))
					deltaUpdateMemory(original.effectResourceMarshalls(), change.effectResourceMarshalls(), this->m_effectResources, memory, "effect_resources");

				if (!memory.isLocked("effect_parameters"))
					deltaUpdateMemory(original.effectParameterMarshalls(), change.effectParameterMarshalls(), this->m_effectParameters, memory, "effect_parameters");
			}

			if (lockAfter)
			{
				memory.lock("effect_resources");
				memory.lock("effect_parameters");
			}
		}

		bool allowUpdate = true;
		bool lockAfter = false;

		// Might be problematic to change the percept, but hopefully the marshalls should capture the change as well
		if (original.percept()->id() != change.percept()->id())
		{
			if (this->percept()->id() == original.percept()->id())
			{
				// If some other mod has changed the marshalls, then we can't change the percept without breaking that
				if (memory.isChanged("percept_resources") || memory.isChanged("percept_parameters"))
				{
					allowUpdate = false;
					lockAfter = true;
				}
				else
				{
					m_percept = change.percept();

					// Prevent other mods from doing marshall updates as they would be dependent on the original transition
					lockAfter = true;
				}
			}
		}

		if (allowUpdate)
		{
			if (!memory.isLocked("percept_resources"))
				deltaUpdateMemory(original.perceptResourceMarshalls(), change.perceptResourceMarshalls(), this->m_perceptResources, memory, "percept_resources");

			if (!memory.isLocked("percept_parameters"))
				deltaUpdateMemory(original.perceptParameterMarshalls(), change.perceptParameterMarshalls(), this->m_perceptParameters, memory, "percept_parameters");
		}

		if (lockAfter)
		{
			memory.lock("percept_resources");
			memory.lock("percept_parameters");
		}
	}

	Effect *Transition::effect() const
	{
		return m_effect;
	}

	void Transition::setEffect(Effect *effect)
	{
		m_effect = effect;

		for (ResourceMarshall *r : m_effectResources)
			delete r;
		for (ParameterMarshall *p : m_effectParameters)
			delete p;

		m_effectResources.clear();
		m_effectParameters.clear();

		if (m_effect != nullptr)
		{
			for (Resource *r : m_effect->resources())
			{
				if (!r->m_internalResource)
					m_effectResources.push_back(new ResourceMarshall(*r));
			}

			for (Parameter *p : m_effect->parameters())
			{
				m_effectParameters.push_back(new ParameterMarshall(*p));
			}
		}
	}

	const std::vector<ResourceMarshall*> &Transition::effectResourceMarshalls() const
	{
		return m_effectResources;
	}

	const std::vector<ParameterMarshall*> &Transition::effectParameterMarshalls() const
	{
		return m_effectParameters;
	}

	Percept *Transition::percept() const
	{
		return m_percept;
	}

	void Transition::setPercept(Percept *percept)
	{
		m_percept = percept;

		for (ResourceMarshall *r : m_perceptResources)
			delete r;
		for (ParameterMarshall *p : m_perceptParameters)
			delete p;

		m_perceptResources.clear();
		m_perceptParameters.clear();

		// Should never be null, but just in case
		if (m_percept != nullptr)
		{
			for (Resource *r : m_percept->resources())
			{
				if (!r->m_internalResource)
					m_perceptResources.push_back(new ResourceMarshall(*r));
			}

			for (Parameter *p : m_percept->parameters())
			{
				m_perceptParameters.push_back(new ParameterMarshall(*p));
			}
		}
	}

	const std::vector<ResourceMarshall*> &Transition::perceptResourceMarshalls() const
	{
		return m_perceptResources;
	}

	const std::vector<ParameterMarshall*> &Transition::perceptParameterMarshalls() const
	{
		return m_perceptParameters;
	}

	const State *Transition::state() const
	{
		return m_state;
	}

	void Transition::setState(State *state)
	{
		m_state = state;
	}

	void Transition::copyFrom(const Transition *other)
	{
		this->setName(other->name());

		this->setEffect(other->effect());
		this->setPercept(other->percept());

		for (size_t i = 0; i < other->effectParameterMarshalls().size(); i++)
		{
			this->effectParameterMarshalls()[i]->m_type = other->effectParameterMarshalls()[i]->m_type;
			this->effectParameterMarshalls()[i]->m_value = other->effectParameterMarshalls()[i]->m_value;
		}

		for (size_t i = 0; i < other->effectResourceMarshalls().size(); i++)
		{
			this->effectResourceMarshalls()[i]->m_type = other->effectResourceMarshalls()[i]->m_type;
			this->effectResourceMarshalls()[i]->m_value = other->effectResourceMarshalls()[i]->m_value;
		}

		for (size_t i = 0; i < other->perceptParameterMarshalls().size(); i++)
		{
			this->perceptParameterMarshalls()[i]->m_type = other->perceptParameterMarshalls()[i]->m_type;
			this->perceptParameterMarshalls()[i]->m_value = other->perceptParameterMarshalls()[i]->m_value;
		}

		for (size_t i = 0; i < other->perceptResourceMarshalls().size(); i++)
		{
			this->perceptResourceMarshalls()[i]->m_type = other->perceptResourceMarshalls()[i]->m_type;
			this->perceptResourceMarshalls()[i]->m_value = other->perceptResourceMarshalls()[i]->m_value;
		}
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

		if (m_state == nullptr)
			throw ATN::Exception("Transition \"%s\" (Unique ID: %d) is not connected to any state", name(), id());

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

	bool Transition::equals(const Entry *otherEntry) const
	{
		if (!Entry::equals(otherEntry))
			return false;

		const Transition *other = (Transition*)otherEntry;

		if (this->state()->id() != other->state()->id())
			return false;

		if (this->effectParameterMarshalls().size() != other->effectParameterMarshalls().size())
			return false;

		if (this->effectResourceMarshalls().size() != other->effectResourceMarshalls().size())
			return false;

		if (this->perceptParameterMarshalls().size() != other->perceptParameterMarshalls().size())
			return false;

		if (this->perceptResourceMarshalls().size() != other->perceptResourceMarshalls().size())
			return false;

		for (size_t i = 0; i < this->effectParameterMarshalls().size(); i++)
		{
			if (*this->effectParameterMarshalls()[i] != *other->effectParameterMarshalls()[i])
				return false;
		}

		for (size_t i = 0; i < this->effectResourceMarshalls().size(); i++)
		{
			if (*this->effectResourceMarshalls()[i] != *other->effectResourceMarshalls()[i])
				return false;
		}

		for (size_t i = 0; i < this->perceptParameterMarshalls().size(); i++)
		{
			if (*this->perceptParameterMarshalls()[i] != *other->perceptParameterMarshalls()[i])
				return false;
		}

		for (size_t i = 0; i < this->perceptResourceMarshalls().size(); i++)
		{
			if (*this->perceptResourceMarshalls()[i] != *other->perceptResourceMarshalls()[i])
				return false;
		}

		return true;
	}
}
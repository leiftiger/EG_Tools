#pragma once

#include "ATN_Entry.h"
#include "ATN_IResourceHolder.h"
#include "ATN_State.h"
#include "ATN_Thread.h"
#include "ATN_Resources.h"

namespace ATN
{
	// Forward declared
	class State; class Thread; class Transition;

	// A network, it serves as a container from which states can access its resources and send it parameters (?)
	class Network : public Entry, IResourceHolder
	{
	protected:

		// The states that will be traversed in the network
		std::vector<State*> m_states;
		std::vector<Thread*> m_threads;

		std::vector<Resource*> m_resources;
		std::vector<Parameter*> m_parameters;

		// Track whether or not the last remove() call was due to a move
		bool m_moving = false;

		virtual void serialize(std::ostream &stream) const override;
		virtual void deserialize(std::istream &stream) override;

		void swapParameterMarshallIndices(std::int64_t index1, std::int64_t index2);
		// Add a new parameter marshall in all states to represent a new parameter being added
		void addParameterMarshall();
		// Remove a specified parameter marshall index from all states and transitions
		void removeParameterMarshall(std::int64_t index);

	public:

		~Network();

		virtual const char * const typeName() const override;

		// Gets all states belonging to this network
		const std::vector<State*> &states() const;

		// Add a new state in the network at the last position
		void add(State &state);
		// Move this state upwards in the state list
		void moveUp(State &state);
		// Move this state downwards in the state list
		void moveDown(State &state);
		// Remove this state from the network
		std::vector<State*>::iterator remove(State &state);

		// Gets all threads belonging to this network
		const std::vector<Thread*> &threads() const;

		// Add a new thread in the network at the last position
		void add(Thread &thread);
		// Move this thread upwards in the thread list
		void moveUp(Thread &thread);
		// Move this thread downwards in the thread list
		void moveDown(Thread &thread);
		// Remove this thread from the network
		std::vector<Thread*>::iterator remove(Thread &thread);

		// Gets all resources belonging to this network
		virtual const std::vector<Resource*> &resources() const override;

		// Add a new resource in the network at the last position
		void add(Resource &resource);
		// Move this resource upwards in the resource list
		void moveUp(Resource &resource);
		// Move this resource downwards in the resource list
		void moveDown(Resource &resource);
		// Remove this resource from the network
		std::vector<Resource*>::iterator remove(Resource &resource);

		// Gets all parameters belonging to this network
		virtual const std::vector<Parameter*> &parameters() const override;

		// Add a new parameter in the network at the last position
		void add(Parameter &param);
		// Move this parameter upwards in the parameter list
		void moveUp(Parameter &param);
		// Move this parameter downwards in the parameter list
		void moveDown(Parameter &param);
		// Remove this parameter from the network
		std::vector<Parameter*>::iterator remove(Parameter &param);
	};
}

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
		void removeParameterMarshalls(std::int64_t index);

		void swapResourceMarshallIndices(std::int64_t index1, std::int64_t index2);

		// Swap input resource marshalls if the input order has changed
		// Swap the transition index for the given resource and the one after it
		void swapResourceMarshallInputs(const Resource &resource);

		// Add a new resource marshall in all states to represent a new resource being added
		void addResourceMarshall();
		// Remove a specified resource marshall index from all states and transitions
		void removeResourceMarshalls(std::int64_t index, const Resource &resource);

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

		// Updates all network transition resource marshalls references to the specified resource
		void updateResourceMarshalls(const Resource &resource);

		// Clears all references to the resource at specified index unless it is still capable of being cast to the specified resource
		void resetResourceMarshalls(std::int64_t index, const Resource &resource);

		// Attempts to point any invalid resource marshall pointer to the first best resource
		// that's in the network or an external transition to it
		void resetInvalidResourceMarshalls();

		// Mark the specified resource as internal and update affected resource marshalls
		void setResourceInternal(Resource &resource, bool internal);

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

		// Clears all references to the parameter at the specified index to a constant value
		void resetParameterMarshalls(std::int64_t index);

		// Returns true if this network has some resources as inputs
		bool hasResourceInputs() const;

		// Counts the number of dependencies of the specified resource
		// in this network and external network transitions
		int countDependencies(Resource &resource) const;

		// Counts the number of dependencies of the specified parameter
		// in this network and external network transitions
		int countDependencies(Parameter &param) const;
	};
}

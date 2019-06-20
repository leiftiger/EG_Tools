#pragma once

#include "ATN_Entry.h"
#include "ATN_State.h"
#include "ATN_Thread.h"
#include "ATN_Type.h"

namespace ATN
{
	// A network, it serves as a container from which states can access its resources and send it parameters (?)
	class Network : protected Entry
	{
	protected:

		// Triple for storing parameters
		class Parameter
		{
		public:

			std::string m_type;
			std::int32_t m_defaultValue;
			std::string m_desc;

			Parameter(std::string type, std::int32_t defaultValue, std::string desc);
		};

		// Triple for resources
		class Resource
		{
		public:

			ResourceType m_type;
			std::string m_desc;
			std::int32_t m_defaultValue;

			Resource(ResourceType type, std::string desc, std::int32_t defaultValue);
		};


		// The states that will be traversed in the network
		std::vector<State*> m_states;
		std::vector<Thread*> m_threads;

		std::vector<Resource> m_resources;
		std::vector<Parameter> m_networkResources;
	};
}

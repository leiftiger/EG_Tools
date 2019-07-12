#pragma once

#include "ATN_Resources.h"

namespace ATN
{
	// Forward declared
	class Parameter; class Resource;

	class IResourceHolder
	{
	public:

		// Gets parameters managed by this object
		virtual const std::vector<Parameter*> &parameters() const = 0;
		// Gets resources managed by this object
		virtual const std::vector<Resource*> &resources() const = 0;
	};
}
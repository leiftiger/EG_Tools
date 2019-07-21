#pragma once

#include <iostream>
#include <fstream>

#include "RL_IResourceLoader.h"
#include "rl_utils.h"

namespace RL
{
	class EntityDescLoader : public IResourceLoader
	{
	protected:
		static const char * const  typeToParameter(const std::string &type);
		static bool includeNumberInType(const std::string &type);

		static int digitCountFromName(const std::string &name);
		static std::int64_t numberFromName(const std::string &name);

	public:
		virtual const char * const extension() const override;

		virtual std::vector<BaseResource*> load(const std::string &filename) override;

	};
}
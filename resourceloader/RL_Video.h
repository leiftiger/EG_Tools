#pragma once

#include "RL_IResourceLoader.h"
#include "rl_utils.h"

namespace RL
{
	class VideoLoader : public IResourceLoader
	{
	public:

		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> load(const std::string &filename) override;
	};
}

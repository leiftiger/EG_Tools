#pragma once

#include "RL_IResourceLoader.h"

#include "RL_FileLoader.h"

#include "rl_utils.h"

#include <fstream>

namespace RL
{
	class GUILoader : public IResourceLoader
	{
	public:
		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> load(const std::string &filename, const FileLoader &loader) override;
	};
}

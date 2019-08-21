#pragma once

#include "RL_IResourceLoader.h"

#include "RL_FileLoader.h"

#include "rl_utils.h"

#include <fstream>

namespace RL
{
	class StringLoader : public IResourceLoader
	{
	protected:

		bool shouldParseFile(const std::string &filename);

	public:

		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> load(const std::string &filename, const FileLoader &loader) override;
	};
}

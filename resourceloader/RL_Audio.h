#pragma once
#include "RL_IResourceLoader.h"

namespace RL
{
	class AudioLoader : public IResourceLoader
	{
	public:
		virtual  const char * const extension() const override;

		virtual std::vector<BaseResource*> load(const std::string &filename, const FileLoader &loader) override;
	};
}
#pragma once

#include "RP_IResourceParser.h"

#include "RP_FileLoader.h"

#include "RP_utils.h"

namespace RL
{
	class VideoParser : public IResourceParser
	{
	public:

		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> parse(const std::string &filename, const FileLoader &loader) override;
	};
}

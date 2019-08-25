#pragma once

#include "RP_IResourceParser.h"

#include "RP_FileLoader.h"

#include "RP_utils.h"

#include <fstream>

namespace RL
{
	class GUIParser : public IResourceParser
	{
	public:
		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> parse(const std::string &filename, const FileLoader &loader) override;
	};
}

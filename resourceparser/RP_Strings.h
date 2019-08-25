#pragma once

#include "RP_IResourceParser.h"

#include "RP_FileLoader.h"

#include "RP_utils.h"

#include <fstream>

namespace RL
{
	class StringParser : public IResourceParser
	{
	protected:

		bool shouldParseFile(const std::string &filename);

	public:

		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> parse(const std::string &filename, const FileLoader &loader) override;
	};
}

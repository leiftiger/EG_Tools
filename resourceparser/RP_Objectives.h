#pragma once
#include "RP_IResourceParser.h"

namespace RL
{
	// Parses Config/ObjectiveDetails.ini for special objective names for progress bars
	class ObjectiveParser : public IResourceParser
	{
	public:

		virtual const char * const extension() const override;
		virtual std::vector<BaseResource*> parse(const std::string &filename, const FileLoader &loader) override;
	};
}

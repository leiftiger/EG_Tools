#pragma once

#include <iostream>
#include <fstream>

#include "RP_IResourceParser.h"

#include "RP_FileLoader.h"

#include "RP_utils.h"

namespace RL
{
	class EntityDescParser : public IResourceParser
	{
	protected:
		static const char * const  typeToParameter(const std::string &type);
		static bool includeNumberInType(const std::string &type);

		static int digitCountFromName(const std::string &name);
		static std::int64_t numberFromName(const std::string &name);

	public:
		virtual const char * const extension() const override;

		virtual std::vector<BaseResource*> parse(const std::string &filename, const FileLoader &loader) override;

	};
}
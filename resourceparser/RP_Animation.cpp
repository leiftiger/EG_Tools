#include "RP_Animation.h"

namespace RL
{
	const char * const AnimationParser::extension() const
	{
		return ".as";
	}

	std::vector<BaseResource*> AnimationParser::parse(const std::string &filename, const FileLoader &loader)
	{
		std::vector<BaseResource*> res;

		std::istream *fs = loader.openFile(filename);

		std::string line;

		// Parse all animation definitions
		while (util::getline(*fs, line))
		{
			if (line.length() >= strlen("*ID_STRING") && line.substr(0, strlen("*ID_STRING")) == "*ID_STRING")
			{
				line = line.substr(strlen("*ID_STRING")+1);
				res.push_back(new BaseResource("Animation", line, util::hashFNV132(line)));
			}
		}

		delete fs;

		return res;
	}
}
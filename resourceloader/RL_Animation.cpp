#include "RL_Animation.h"

namespace RL
{
	const char * const AnimationLoader::extension() const
	{
		return ".as";
	}

	std::vector<BaseResource*> AnimationLoader::load(const std::string &filename)
	{
		std::vector<BaseResource*> res;

		std::ifstream file(filename);

		std::string line;

		// Parse all animation definitions
		while (std::getline(file, line))
		{
			if (line.length() >= strlen("*ID_STRING") && line.substr(0, strlen("*ID_STRING")) == "*ID_STRING")
			{
				line = line.substr(strlen("*ID_STRING")+1);
				res.push_back(new BaseResource("Animation", line, utils::hashElixir(line)));
			}
		}

		return res;
	}
}
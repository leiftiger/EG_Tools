#include "RP_Objectives.h"


namespace RL
{
	const char * const ObjectiveParser::extension() const
	{
		return ".ini";
	}

	std::vector<BaseResource*> ObjectiveParser::parse(const std::string &filename, const FileLoader &loader)
	{
		std::vector<BaseResource*> res;

		if (IResourceParser::pathToFileName(filename) != "ObjectiveDetails")
			return res;

		std::istream *fs = loader.openFile(filename);

		std::string line;

		while (util::getline(*fs, line))
		{
			int indexEnd = (int)line.find_first_of(' ');

			if (indexEnd == -1)
				continue;

			std::string objective = line.substr(0, indexEnd);

			res.push_back(new BaseResource("Objective ID", objective, util::hashFNV132(objective)));
		}

		delete fs;

		return res;
	}
}

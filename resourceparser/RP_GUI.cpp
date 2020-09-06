#include "RP_GUI.h"

#include <cstring>

namespace RL
{
	const char * const GUIParser::extension() const
	{
		return ".csb";
	}
	std::vector<BaseResource*> GUIParser::parse(const std::string &filename, const FileLoader &loader)
	{
		std::vector<BaseResource*> res;

		std::string state = IResourceParser::pathToFileName(filename);

		// State appears to be the file defining all controls
		res.push_back(new BaseResource("UI State", state, util::hashFNV132(state)));

		std::istream *fs = loader.openFile(filename);

		std::string line;

		while (util::getline(*fs, line))
		{
			if ((int)line.find("Name=") == 0)
			{
				std::string name = line.substr(strlen("Name="));

				res.push_back(new BaseResource("GUI Control", name, util::hashFNV132(name)));
			}
		}

		delete fs;

		return res;
	}
}

#include "RP_Audio.h"

#include <cstring>

namespace RL
{
	const char * const AudioParser::extension() const
	{
		return ".spb";
	}

	std::vector<BaseResource*> AudioParser::parse(const std::string &filename, const FileLoader &loader)
	{
		std::vector<BaseResource*> res;

		std::istream *fs = loader.openFile(filename);

		std::string line;
		std::string lastSupClass;
		std::string lastSubClass;

		// TODO: Determine what sound IDs are

		// Parse all animation definitions
		while (util::getline(*fs, line))
		{
			if (line.length() >= strlen("Class=") && line.substr(0, strlen("Class=")) == "Class=")
			{
				lastSupClass = line.substr(strlen("Class="));
			}

			if (line.length() >= strlen("Subclass=") && line.substr(0, strlen("Subclass=")) == "Subclass=")
			{
				lastSubClass = line.substr(strlen("Subclass="));
			}
			if (line.length() >= strlen("Category=") && line.substr(0, strlen("Category=")) == "Category=")
			{
				line = line.substr(strlen("Category="));

				std::string strSound = lastSupClass + "-" + lastSubClass + "-" + line;

				//res.push_back(new BaseResource("Sound ID", strSound, util::hashFNV132(strSound)));
			}
		}

		delete fs;

		return res;
	}
}
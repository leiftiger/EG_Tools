#include "RL_StringLoader.h"

namespace RL
{
	bool StringLoader::shouldParseFile(const std::string &filename)
	{
		int indices[] =
		{
			(int)filename.find("Strings_DefeatSA"),
			(int)filename.find("Strings_Henchman_Recruit"),
			(int)filename.find("Strings_Objective"),
			(int)filename.find("Strings_Objective"),
			(int)filename.find("Strings_Research"),
		};

		for (int index : indices)
		{
			if (index >= 0)
				return true;
		}

		return false;
	}

	const char * const StringLoader::extension() const
	{
		return ".txt";
	}

	std::vector<BaseResource*> StringLoader::load(const std::string &filename)
	{
		std::vector<BaseResource*> res;

		if (!shouldParseFile(IResourceLoader::pathToFileName(filename)) && filename.rfind("Strings.txt") == -1)
			return res;

		std::ifstream file(filename);

		std::string lineCur;
		std::string linePrev;

		bool isObjective = false;


		while (std::getline(file, lineCur))
		{
			if ((int)lineCur.find("##") >= 0)
			{
				isObjective = false;

				if (lineCur == "## Objective Screen Entries")
					isObjective = true;
			}

			if (lineCur == "<BEGIN>")
			{
				int index;

				index = (int)linePrev.find("EVENT_");

				if (index >= 0)
				{
					res.push_back(new BaseResource("Event", linePrev, util::hashFNV132(linePrev)));
					res.push_back(new BaseResource("String ID", linePrev, util::hashFNV132(linePrev)));
				}

				index = (int)linePrev.find("OBJECTIVE_");

				if (index >= 0)
				{
					const char *strSave = "_SAVEFILE";

					// Not relevant for ATNs
					if (linePrev.length() > strlen(strSave) && linePrev.substr(linePrev.length() - strlen(strSave)) == strSave)
						continue;

					if (isObjective)
						res.push_back(new BaseResource("Objective ID", linePrev, util::hashFNV132(linePrev)));
					else
						res.push_back(new BaseResource("String ID", linePrev, util::hashFNV132(linePrev)));
				}
			}

			linePrev = lineCur;
		}

		return res;
	}
}
#include "RL_StringLoader.h"

namespace RL
{
	bool StringLoader::shouldParseFile(const std::string &filename)
	{
		long indices[] =
		{
			filename.find("Strings_DefeatSA"),
			filename.find("Strings_Henchman_Recruit"),
			filename.find("Strings_Objective"),
			filename.find("Strings_Objective"),
			filename.find("Strings_Research"),
		};

		for (size_t index : indices)
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


		while (std::getline(file, lineCur))
		{
			if (lineCur == "<BEGIN>")
			{
				long index;

				index = linePrev.find("EVENT_");

				if (index >= 0)
				{
					res.push_back(new BaseResource("Event", linePrev, util::hashFNV132(linePrev)));
					res.push_back(new BaseResource("String ID", linePrev, util::hashFNV132(linePrev)));
				}

				index = linePrev.find("OBJECTIVE_");

				if (index >= 0)
				{
					const char *strSave = "_SAVEFILE";

					// Not relevant for ATNs
					if (linePrev.length() > strlen(strSave) && linePrev.substr(linePrev.length() - strlen(strSave)) == strSave)
						continue;

					res.push_back(new BaseResource("Objective ID", linePrev, util::hashFNV132(linePrev)));
				}
			}

			linePrev = lineCur;
		}

		return res;
	}
}
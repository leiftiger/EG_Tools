#include "RL_EntityDesc.h"

namespace RL
{
	const char * const EntityDescLoader::typeToParameter(const std::string &type)
	{
		if (type == "ActOfInfamy")
			return "Act Of Infamy ID";
		else if (type == "AgentGroup")
			return "Agent Group Type";
		else if (type == "Room")
			return "Room Type";
		else if (type == "WorldRegion")
			return "World Map Region";

		return nullptr;
	}

	bool EntityDescLoader::includeNumberInType(const std::string &type)
	{
		if (type == "ActOfInfamy")
			return true;
		else if (type == "AgentGroup")
			return false;
		else if (type == "Room")
			return false;
		else if (type == "WorldRegion")
			return false;

		return true;
	}

	int EntityDescLoader::digitCountFromName(const std::string &name)
	{
		int digits = 0;

		for (size_t i = 0; i < name.length(); i++)
		{
			if (isdigit(name[i]))
				digits++;
			else
				break;
		}

		return digits;
	}

	std::int64_t EntityDescLoader::numberFromName(const std::string &name)
	{
		int digits = digitCountFromName(name);

		return std::stoll(name.substr(0, digits));
	}

	const char * const EntityDescLoader::extension() const
	{
		return ".desc";
	}
	std::vector<BaseResource*> EntityDescLoader::load(const std::string &filename)
	{
		std::ifstream file(filename);

		std::string line;

		std::getline(file, line);

		std::vector<BaseResource*> res;

		std::string type = line.substr(strlen("ObjType="));

		// Things like room walls which will probably never be relevant in the context of ATNs
		if (type == "Scenery")
			return res;

		std::string name = IResourceLoader::pathToFileName(filename);

		std::int64_t value = numberFromName(name);

		size_t numEnd = digitCountFromName(name)-1;

		if (name[numEnd + 1] == '-')
			name = name.substr(numEnd + 2);
		// World regions seem to have an extra space for some reason
		else if (name[numEnd + 2] == '-')
			name = name.substr(numEnd + 4);

		res.push_back(new BaseResource("Entity Type", std::to_string(value) + ": " + name, value));

		const char * const paramType = typeToParameter(type);

		if (paramType != nullptr)
		{
			if (includeNumberInType(type))
			{
				name = std::to_string(value) + ": " + name;
			}

			res.push_back(new BaseResource(paramType, name, value));
		}

		// Parse any hotspots defined in the file
		while (std::getline(file, line))
		{
			if (line.length() >= strlen("HotspotName=") && line.substr(0, strlen("HotspotName=")) == "HotspotName=")
			{
				line = line.substr(strlen("HotspotName="));
				res.push_back(new BaseResource("Hotspot", line, utils::hashElixir(line)));
			}
		}

		return res;
	}
}
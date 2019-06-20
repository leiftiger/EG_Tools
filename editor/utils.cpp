
#include "utils.h"

#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

#include "ATN_Effect.h"
#include "ATN_Network.h"
#include "ATN_Percept.h"
#include "ATN_State.h"
#include "ATN_Thread.h"
#include "ATN_Transition.h"

#include "ATN_Exception.h"

static const std::uint32_t prime = 16777619;

// Taken from the lead programmer's response as well as Dameon's notes on the Evil Planet forums: http://web.archive.org/web/20111122124528/http://n1nj4.com/viewtopic.php?p=120931#120931
std::uint32_t util::hashElixir(const std::string &str)
{
	// Hash is always of the lowercased string
	std::string lCase = str;

	std::transform(lCase.begin(), lCase.end(), lCase.begin(), ::tolower);

	std::uint8_t* buff = (std::uint8_t*)lCase.c_str();
	int buffLength = lCase.length();

	std::int32_t hash = 2166136261;

	for (int i = 0; i < buffLength; ++i)
	{
		hash *= prime;
		hash ^= std::int32_t(*buff);

		buff++;
	}

	return hash;
}

ATN::List<ATN::Property> util::parseEvents(const std::string &filename)
{
	std::ifstream file(filename);

	if (file.fail())
	{
		throw ATN::Exception("Couldn't find file %s", filename);
	}

	std::string line;

	ATN::List<ATN::Property> list;

	while (getline(file, line))
	{
		ATN::Property *el = new ATN::Property(line, hashElixir(line));

		std::cout << el->name() << "\t" << el->id() << std::endl;

		list.add(*el);
	}

	return list;
}

ATN::List<ATN::Entry> util::parseATN(const std::string &filename)
{
	std::ifstream file(filename);

	if (file.fail())
	{
		throw ATN::Exception("Couldn't find file %s", filename);
	}

	std::string line;

	getline(file, line);

	if (line != "[Header]")
	{
		throw ATN::Exception("Invalid ATN file, missing \"[Header]\"");
	}

	getline(file, line);

	line = line.substr(strlen("NumElements="));

	int numObjects = std::stoi(line);

	getline(file, line);

	if (line != "Identifier=ATNData")
	{
		throw ATN::Exception("Invalid ATN file, missing \"Identifier=ATNData\"");
	}

	getline(file, line); // blank line
	getline(file, line);

	if (line != "[Object headers]")
	{
		throw ATN::Exception("Invalid ATN file, missing \"[Object headers]\"");
	}

	ATN::List<ATN::Entry> list;

	for (int i = 0; i < numObjects; i++)
	{
		getline(file, line); // redundant info (ObjectNum)
		getline(file, line);

		std::string objType = line.substr(strlen("TypeName="));

		ATN::Entry *el;

		if (objType == "TATNNetwork")
		{
			el = (ATN::Entry*)new ATN::Network();
		}
		else if (objType == "TATNThread")
		{
			el = (ATN::Entry*)new ATN::Thread();
		}
		else if (objType == "TATNState")
		{
			el = (ATN::Entry*)new ATN::State();
		}
		else if (objType == "TATNStateTransition")
		{
			el = (ATN::Entry*)new ATN::Transition();
		}
		else if (objType.find("TATNEffect"))
		{
			el = (ATN::Entry*)new ATN::Effect(objType);
		}
		else if (objType.find("TATNPercept"))
		{
			el = (ATN::Entry*)new ATN::Percept(objType);
		}
		else
		{
			throw ATN::Exception("Couldn't interpret type of \"%s\"", objType);
		}

		getline(file, line);

		int objID = std::stoi(line.substr(strlen("UniqueID=")));

		el->setID(objID);

		list.add(*el);

		getline(file, line); // blank line
	}

	if (line != "[Object data]")
	{
		throw ATN::Exception("Invalid ATN file, missing \"[Object data]\"");
	}

	for (int i = 0; i < numObjects; i++)
	{
		getline(file, line); // redundant typename
		getline(file, line);

		int objID = std::stoi(line.substr(strlen("UniqueID=")));

		ATN::Entry &el = list.find(objID);

		file >> el;
	}

	return list;
}
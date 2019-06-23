
#include "utils.h"

#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

#include "ATN_Effect.h"
#include "ATN_Network.h"
#include "ATN_Object.h"
#include "ATN_Percept.h"
#include "ATN_State.h"
#include "ATN_Thread.h"
#include "ATN_Transition.h"

#include "ATN_Exception.h"

namespace util
{
	int DEBUG_LINE = -1;

	static const std::uint32_t prime = 16777619;

	// Taken from the lead programmer's response as well as Dameon's notes on the Evil Planet forums: http://web.archive.org/web/20111122124528/http://n1nj4.com/viewtopic.php?p=120931#120931
	std::int32_t hashElixir(const std::string &str)
	{
		// Hash is always of the lowercased string
		std::string lCase = str;

		std::transform(lCase.begin(), lCase.end(), lCase.begin(), ::tolower);

		std::uint8_t* buff = (std::uint8_t*)lCase.c_str();
		int buffLength = lCase.length();

		// We hash using unsigned, to avoid any issues with the multiplication
		// then we just return the signed value that is seemingly used in the ATNs
		std::uint32_t hash = 2166136261;

		for (int i = 0; i < buffLength; ++i)
		{
			hash *= prime;
			hash ^= std::uint32_t(*buff);

			buff++;
		}

		return hash;
	}

	std::istream &getline(std::istream &stream, std::string &line)
	{
		DEBUG_LINE++;

		return std::getline(stream, line);
	}

	ATN::List<ATN::Property> parseEvents(const std::string &filename)
	{
		DEBUG_LINE = -1;

		std::ifstream file(filename);

		if (file.fail())
		{
			throw ATN::Exception("Couldn't find file %s", filename);
		}

		std::string line;

		ATN::List<ATN::Property> list(filename);

		while (getline(file, line))
		{
			ATN::Property *el = new ATN::Property(line, hashElixir(line));

			//std::cout << el->name() << "\t" << el->id() << std::endl;

			list.add(*el);
		}

		util::DEBUG_LINE = -1;

		return list;
	}

	// Parse ATN values from specified text file
	// outList: list whose entries we'll add or update
	// secondPass: if true, no entires will be added, but references will be updated
	void parseATN(const std::string &filename, ATN::List<ATN::Entry> &outList, bool secondPass)
	{
		DEBUG_LINE = -1;

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

		for (int i = 0; i < numObjects; i++)
		{
			getline(file, line); // redundant info (ObjectNum)
			getline(file, line);

			std::string objType = line.substr(strlen("TypeName="));

			ATN::Entry *el = nullptr;

			if (!secondPass)
			{
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
				else if (objType == "TATNObjectType")
				{
					el = (ATN::Entry*)new ATN::Object();
				}
				else if (objType.find("TATNEffect") != -1)
				{
					el = (ATN::Entry*)new ATN::Effect(objType);
				}
				else if (objType.find("TATNPercept") != -1)
				{
					el = (ATN::Entry*)new ATN::Percept(objType);
				}
				else
				{
					throw ATN::Exception("Couldn't interpret type of \"%s\"", objType);
				}
			}

			getline(file, line);

			int objID = std::stoi(line.substr(strlen("UniqueID=")));

			if (!secondPass)
			{
				el->setID(objID);

				outList.add(*el);
				outList.recordOrderHeader(*el);
			}

			getline(file, line); // blank line
		}

		getline(file, line);

		if (line != "[Object data]")
		{
			throw ATN::Exception("Invalid ATN file, missing \"[Object data]\"");
		}

		if (secondPass)
		{
			for (int i = 0; i < numObjects; i++)
			{
				getline(file, line); // redundant typename
				getline(file, line);

				int objID = std::stoi(line.substr(strlen("UniqueID=")));

				ATN::Entry &el = outList.find(objID);

				outList.recordOrderData(el);
				outList.updateName(el);

				file >> el;
			}
		}
		else
		{
			ATN::Manager::addList(&outList);
		}

		util::DEBUG_LINE = -1;
	}

	// Write ATN values to specified text file
	// outList: list whose entries we'll add or update
	// secondPass: if true, no entires will be added, but references will be updated
	void writeATN(const std::string &filename, const ATN::List<ATN::Entry> &list)
	{
		DEBUG_LINE = -1;

		std::ofstream stream(filename, std::ios::trunc);

		if (stream.fail())
		{
			throw ATN::Exception("Couldn't create file %s", filename);
		}

		stream << "[Header]" << std::endl;

		stream << "NumElements=" << list.size() << std::endl;

		stream << "Identifier=ATNData" << std::endl << std::endl;

		stream << "[Object headers]" << std::endl;

		std::vector<ATN::Entry*> fileOrderHeader = list.getWriteOrderHeader();
		std::vector<ATN::Entry*> fileOrderData = list.getWriteOrderData();

		for (size_t i = 0; i < fileOrderHeader.size(); i++)
		{
			stream << "ObjectNum=" << i << std::endl;
			stream << "TypeName=" << fileOrderHeader[i]->typeName() << std::endl;
			stream << "UniqueID=" << fileOrderHeader[i]->id() << std::endl << std::endl;
		}

		stream << "[Object data]" << std::endl;

		for (ATN::Entry *el : fileOrderData)
		{
			stream << *el;
		}
	}
}
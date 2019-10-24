
#include "utils.h"

#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

#include "ATN/ATN_Effect.h"
#include "ATN/ATN_Network.h"
#include "ATN/ATN_Object.h"
#include "ATN/ATN_Percept.h"
#include "ATN/ATN_State.h"
#include "ATN/ATN_Thread.h"
#include "ATN/ATN_Transition.h"

#include "ATN/ATN_Exception.h"

namespace util
{
	int DEBUG_LINE = -1;

	static const std::uint32_t prime = 16777619;

	// Fowler-Noll-Vo 1 32 bit hash function
	std::int32_t hashFNV132(const std::string &str)
	{
		// Hash is always of the lowercased string
		std::string lCase = str;

		std::transform(lCase.begin(), lCase.end(), lCase.begin(), ::tolower);

		std::uint8_t *buff = (std::uint8_t*)lCase.c_str();
		size_t buffLength = lCase.length();

		// We hash using unsigned, to avoid any issues with the multiplication
		// then we just return the signed value that is seemingly used in the ATNs
		std::uint32_t hash = 2166136261;

		for (size_t i = 0; i < buffLength; ++i)
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

		std::getline(stream, line);

		if (line.length() > 0 && line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);

		return stream;
	}

	std::vector<std::string> configPaths(const std::string &filename)
	{
		std::ifstream file(filename);

		std::vector<std::string> res;

		if (file.fail())
		{
			return res;
		}

		std::string line;

		while (getline(file, line))
		{
			res.push_back(line);
		}

		return res;
	}

	ATN::List<ATN::Property> parseHashes(const std::string &filename)
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
			// Ignore comments and empty lines
			if (line.length() == 0 || (line.length() >= 2 && line.substr(0, 2) == "//"))
			{
				continue;
			}

			ATN::Property *el = new ATN::Property(line, hashFNV132(line));

			//std::cout << el->name() << "\t" << el->id() << std::endl;

			list.add(*el);
		}

		util::DEBUG_LINE = -1;

		return list;
	}

	ATN::List<ATN::Property> createDefinition(const std::initializer_list<std::pair<std::string, std::int32_t>> &list)
	{
		ATN::List<ATN::Property> defList;

		bool isFlag = false;

		if (list.size() >= 3)
		{
			if (list.begin()[0].second * 2 == list.begin()[1].second && list.begin()[1].second * 2 == list.begin()[2].second)
			{
				isFlag = true;
			}
		}

		for (const std::pair<std::string, std::int32_t> &pair : list)
		{
			ATN::Property *el = new ATN::Property(pair.first, pair.second);

			defList.add(*el);
		}

		if (isFlag)
		{
			// We add up to 3 combinations, as more may be illegible
			for (const std::pair<std::string, std::int32_t> &pair1 : list)
			{
				for (const std::pair<std::string, std::int32_t> &pair2 : list)
				{
					if (pair1 == pair2)
						continue;

					for (const std::pair<std::string, std::int32_t> &pair3 : list)
					{
						if (pair1 == pair3 || pair2 == pair3)
							continue;

						std::int32_t value = pair1.second | pair2.second | pair3.second;

						if (!defList.contains(value))
							defList.add(*new ATN::Property(pair1.first + "|" + pair2.first + "|" + pair3.first, value));
					}

					if (!defList.contains(pair1.second | pair2.second))
						defList.add(*new ATN::Property(pair1.first + "|" + pair2.first, pair1.second | pair2.second));
				}
			}

			if (!defList.contains(0))
				defList.add(*new ATN::Property("NONE", 0));
		}
		
		return defList;
	}

	ATN::List<ATN::Property> createDefinition(const std::vector<std::pair<std::string, std::int64_t>> &list)
	{
		ATN::List<ATN::Property> defList;

		for (const std::pair<std::string, std::int64_t> &pair : list)
		{
			ATN::Property *el = new ATN::Property(pair.first, (std::int32_t)pair.second);

			defList.add(*el);
		}

		return defList;
	}

	std::vector<std::pair<std::string, std::string>> parseInterpretations(const std::string &filename)
	{
		std::ifstream file(filename);

		if (file.fail())
		{
			throw ATN::Exception("Couldn't find file %s", filename);
		}

		std::vector<std::pair<std::string, std::string>> list;

		std::string line;

		// First two lines are reserved for comments
		getline(file, line);
		getline(file, line);

		while (std::getline(file, line))
		{
			std::string type = line;
			std::string format;

			std::getline(file, format);

			list.push_back(std::make_pair(type, format));

			// Blank line for readability
			std::getline(file, line);
		}

		return list;
	}

	// Parse ATN values from specified stream
	// outList: list whose entries we'll add or update
	// secondPass: if true, no entires will be added, but references will be updated
	void parseATN(std::istream &file, ATN::List<ATN::Entry> &outList, bool secondPass)
	{
		DEBUG_LINE = -1;

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
				outList.registerName(el);

				file >> el;
			}
		}

		util::DEBUG_LINE = -1;
	}

	// Write ATN values to specified stream
	// outList: list whose entries to write
	void writeATN(std::ostream &stream, const ATN::List<ATN::Entry> &list)
	{
		DEBUG_LINE = -1;

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
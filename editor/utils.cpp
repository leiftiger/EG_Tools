
#include "utils.h"

#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

static const std::uint32_t prime = 16777619;

// Taken from the lead programmer's response on the Evil Planet forums: http://web.archive.org/web/20111122124528/http://n1nj4.com/viewtopic.php?p=120931#120931
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
		char err[32];

		sprintf_s(err, "Couldn't find file %s", filename.c_str());

		throw std::exception(err);
	}

	std::string line;

	ATN::List<ATN::Property> list;

	while (getline(file, line))
	{
		std::shared_ptr<ATN::Property> el = std::make_shared<ATN::Property>(line, hashElixir(line));

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
		char err[32];

		sprintf_s(err, "Couldn't find file %s", filename.c_str());

		throw std::exception(err);
	}

	std::string line;

	getline(file, line);

	if (line != std::string("[Header]"))
	{
		throw std::exception("Invalid ATN file, missing \"[Header]\".");
	}

	getline(file, line);

	line = line.substr(strlen("NumElements="));

	int numObjects = std::stoi(line);

	getline(file, line);

	if (line != std::string("Identifier=ATNData"))
	{
		throw std::exception("Invalid ATN file, missing \"Identifier=ATNData\".");
	}

	getline(file, line); // blank line
	getline(file, line);

	if (line != std::string("[Object headers]"))
	{
		throw std::exception("Invalid ATN file, missing \"[Object headers]\".");
	}

	ATN::List<ATN::Entry> list;

	for (int i = 0; i < numObjects; i++)
	{

	}

	while (getline(file, line))
	{
		std::shared_ptr<ATN::Entry> el = std::make_shared<ATN::Entry>();

		std::cout << el->name() << "\t" << el->id() << std::endl;

		list.add(*el);
	}

	return list;
}
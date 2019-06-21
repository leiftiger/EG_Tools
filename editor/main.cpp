#include <iostream>
#include "utils.h"

int main()
{
	try
	{
		std::cout << "Parsing\n";

		util::parseEvents("files/events.txt");

		ATN::List<ATN::Entry> entries;
		util::parseATN("files/ATNData.ros", entries, false);
		util::parseATN("files/ATNData_Objects.ros", entries, false);
		util::parseATN("files/ATNData_Tutorials.ros", entries, false);
		util::parseATN("files/ATNData.ros", entries, true);
		util::parseATN("files/ATNData_Objects.ros", entries, true);
		util::parseATN("files/ATNData_Tutorials.ros", entries, true);
	}
	catch (std::exception &e)
	{
		std::cout << "An unexpected error occured:" << std::endl;
		std::cout << e.what() << std::endl;
	}

	std::cout << "end" << std::endl;
}
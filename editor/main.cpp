#include <iostream>
#include "utils.h"

int main()
{
	try
	{
		std::cout << "Parsing\n";

		util::parseEvents("files/events.txt");

		ATN::List<ATN::Entry> entries = util::parseATN("files/ATNData.ros");
	}
	catch (std::exception &e)
	{
		std::cout << "An unexpected error occured:" << std::endl;
		std::cout << e.what() << std::endl;
	}

	std::cout << "end" << std::endl;
}
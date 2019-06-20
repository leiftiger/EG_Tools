#include <iostream>
#include "utils.h"

int main()
{
	try
	{
		std::cout << "Parsing\n";

		util::parseEvents("files/events.txt");
	}
	catch (std::exception e)
	{
		std::cout << "An unexpected error occured:" << std::endl;
		std::cout << e.what() << std::endl;
	}

	std::cout << "end" << std::endl;
}
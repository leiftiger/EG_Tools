#include <iostream>
#include "utils.h"

#include <QApplication>

#include "UI/UI_MainWindow.h"

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	try
#endif
	{
		std::cout << "Parsing\n";

		ATN::List<ATN::Property> events = util::parseEvents("files/events.txt");

		// TODO: More than events use hashed values in ATN files (objective IDs, objective descriptions and more)
		ATN::Manager::setHashValues(events);

		QApplication a(argc, argv);
		UI_MainWindow w;
		w.show();

		return a.exec();
	}
#ifndef _DEBUG
	catch (std::exception &e)
	{
		std::cout << "An unexpected error occured:" << std::endl;
		std::cout << e.what() << std::endl;
	}
#endif

	std::cout << "end" << std::endl;
}
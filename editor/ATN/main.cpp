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

		// TODO: string hashes (objective and hint strings)
		ATN::Manager::setHashValues("Animation", util::parseHashes("files/anims.txt"));
		ATN::Manager::setHashValues("Event", util::parseHashes("files/events.txt"));

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
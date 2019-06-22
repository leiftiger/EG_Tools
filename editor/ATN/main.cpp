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

		std::vector<std::string> files = {"files/ATNData.ros", "files/ATNData_Objects.ros", "files/ATNData_Tutorials.ros"};
		std::vector<ATN::List<ATN::Entry>*> atns;

		for (const std::string &file : files)
		{
			ATN::List<ATN::Entry> *entries = new ATN::List<ATN::Entry>(file);

			atns.push_back(entries);

			util::parseATN(file, *entries, false);
		}

		for (size_t i = 0; i < files.size(); i++)
		{
			util::parseATN(files[i], *atns[i], true);
		}

		// Since names are added in the second pass, but the global ATN list required all IDs for that to run, we add the names afterwards here (for quick look-up)
		for (size_t i = 1; i < ATN::Manager::lists().size(); i++)
		{
			for (const std::pair<std::uint32_t, ATN::IATN_Data*> &pair : *ATN::Manager::lists()[i])
				ATN::Manager::lists()[0]->updateName(*(ATN::Entry*)pair.second);
		}

		for (size_t i = 0; i < files.size(); i++)
		{
			std::string filename = files[i] + "_out";
			util::writeATN(filename, *atns[i]);
		}

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
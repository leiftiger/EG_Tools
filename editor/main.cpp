#include <iostream>

#include <QApplication>

#include "UI/UI_MainWindow.h"
#include "UI/UI_ErrorWindow.h"

#include "ATN_definitions.h"

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	try
#endif
	{

		util::loadStaticDefinitions();
		util::loadDynamicDefinitions();

		QApplication a(argc, argv);
		UI_MainWindow w;
		w.show();

		return a.exec();
	}
#ifndef _DEBUG
	catch (std::exception &e)
	{
		QApplication a(argc, argv);
		UI_ErrorWindow w;
		w.setErrorMessage(e.what());
		w.show();

		return a.exec();
	}
#endif
}
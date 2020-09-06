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

		// Currently many UI elements are dependent on no DPI-scaling
		QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, false);

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

		std::string strError(e.what());

		if ((int)strError.find("/DynamicResources/") != -1)
		{
			strError += "\n\nThis is likely due to a mod that has been uninstalled, please launch the game again before opening the ATN Editor.";
		}

		w.setErrorMessage(strError);
		w.show();

		return a.exec();
	}
#endif
}
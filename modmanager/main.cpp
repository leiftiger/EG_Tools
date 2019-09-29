#include "UI_MainWindow.h"
#include "UI_ErrorWindow.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	try
#endif
	{
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

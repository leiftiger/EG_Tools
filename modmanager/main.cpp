#include "UI_MainWindow.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	UI_MainWindow w;
	w.show();
	return a.exec();
}

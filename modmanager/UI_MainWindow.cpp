#include "UI_MainWindow.h"
#include <QMessageBox>

UI_MainWindow::UI_MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	populateLists();
}

void UI_MainWindow::populateLists()
{
	ui.listWidgetEnabled->clear();
	ui.listWidgetDisabled->clear();

	if (true)
	{
		QMessageBox msg;

		msg.setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::WindowStaysOnTopHint);
		msg.setIcon(QMessageBox::Icon::Question);

		msg.setWindowTitle(tr(" "));
		msg.setText(tr("<span style=\"font-size:12pt;\"><b>Unmanaged mods</b></span>"));

		msg.setInformativeText(tr("There are some mods installed that aren't managed by the Mod Manager, do you want to make them managed now?\n\nNot doing this could potentially cause conflicts that the Mod Manager is designed to counteract."));

		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setDefaultButton(QMessageBox::Yes);

		QApplication::beep();
		int msgRet = msg.exec();

		switch (msgRet)
		{
		case QMessageBox::Yes:
		{
			break;
		}
		case QMessageBox::No:
		{
			break;
		}
		}
	}
}

void UI_MainWindow::installMods()
{

}

void UI_MainWindow::uninstallMods()
{

}

void UI_MainWindow::saveModInfo()
{

}

void UI_MainWindow::openAboutWindow()
{

}

void UI_MainWindow::openConfigWindow()
{

}

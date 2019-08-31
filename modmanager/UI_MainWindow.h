#pragma once

#include <QtWidgets/QMainWindow>

#include "GeneratedFiles/ui_UI_MainWindow.h"

class UI_MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	UI_MainWindow(QWidget *parent = Q_NULLPTR);

protected slots:
	void populateLists();

	void installMods();
	void uninstallMods();

	void saveModInfo();

	void openAboutWindow();
	void openConfigWindow();

private:
	Ui::UI_MainWindowClass ui;
};

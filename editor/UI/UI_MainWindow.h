#pragma once

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_UI_MainWindow.h"

#include "ATN/ATN_Network.h"

class UI_MainWindow : public QMainWindow
{
	Q_OBJECT

protected:
	std::vector<std::string> m_atnFiles;

	void reloadFileList();

public:
	UI_MainWindow(QWidget *parent = Q_NULLPTR);

	void setNetworkResults(std::vector<ATN::Network*> results);
	void createNetworkResourceTab(ATN::Network &el);

public slots:

	void openFiles();

	void searchName(const QString &str);
	void searchID(const QString &str);

	void openNetworkButton();

	void closeTab(int tab);

private:
	Ui::UI_MainWindowClass ui;
};

#pragma once

#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_UI_MainWindow.h"

#include "UI_HashWindow.h"
#include "UI_ErrorWindow.h"

#include <QMessageBox>

#include "ATN/ATN_Network.h"

class UI_MainWindow : public QMainWindow
{
	Q_OBJECT

protected:
	std::vector<std::string> m_atnFiles;

	UI_HashWindow m_hashWindow;

	UI_ErrorWindow m_errorWindow;

	void reloadFileList();

public:
	UI_MainWindow(QWidget *parent = Q_NULLPTR);

	void setNetworkResults(std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>> results);
	void createNetworkResourceTab(ATN::Network &el);

	virtual void closeEvent(QCloseEvent *event) override;

public slots:

	void newFile();
	void openFiles();
	void saveFiles();

	void newNetwork();

	void openHashTool();

	void searchName(const QString &str);
	void searchID(const QString &str);

	void copyNetworkButton();
	void openNetworkButton();

	void closeTab(int tab);

	void receiveOpenNetworkRequest(int id);
	void receiveCloseNetworkRequest(int id);
	void receiveTransitionsRequest(int id);

	void receiveRepopulationRequest(int id);

private:
	Ui::UI_MainWindowClass ui;
};

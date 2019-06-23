#include "UI_MainWindow.h"

#include "ATN/ATN_Entry.h"
#include "ATN/ATN_List.h"
#include "ATN/ATN_Manager.h"

#include <typeinfo>

#include <QFileDialog>

UI_MainWindow::UI_MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Disable close button on search results tab
	ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
}

void UI_MainWindow::setNetworkResults(std::vector<ATN::Network*> results)
{
	m_curResults = results;


}

void UI_MainWindow::createNetworkResourceTab(ATN::Network &el)
{

}

void UI_MainWindow::reloadFileList()
{
	ui.comboATNList->clear();

	for (const std::string &file : m_atnFiles)
	{
		char filename[128];
		char fileexst[16];

		_splitpath_s(file.c_str(), nullptr, 0, nullptr, 0, filename, sizeof(filename)/sizeof(char), fileexst, 16);

		ui.comboATNList->addItem(QString(filename)+QString(fileexst));
	}
}

void UI_MainWindow::openFiles()
{
	QStringList qFiles = QFileDialog::getOpenFileNames(this, tr("Load ATNs"), "", tr("ATN files (*.ros)"));

	std::vector<std::string> strFiles;

	for (QString str : qFiles)
	{
		strFiles.push_back(str.toStdString());

		m_atnFiles.push_back(str.toStdString());
	}

	ATN::Manager::loadFromFiles(strFiles);

	reloadFileList();
}

void UI_MainWindow::search(const QString &str)
{
	std::vector<std::pair<ATN::Entry*, ATN::List<ATN::Entry>*>> res = ATN::Manager::searchName(str.toStdString());

	std::vector<ATN::Network*> netResults;

	for (std::pair<ATN::Entry*, ATN::List<ATN::Entry>*> &pair : res)
	{
		if (typeid(*pair.first) == typeid(ATN::Network))
		{
			netResults.push_back((ATN::Network*)pair.first);
		}
	}

	setNetworkResults(netResults);
}

void UI_MainWindow::search(int id)
{
	try
	{
		ATN::Entry &el = ATN::Manager::findByID(id);

		if (typeid(el) == typeid(ATN::Network))
		{
			std::vector<ATN::Network*> results;

			results.push_back((ATN::Network*)&el);

			setNetworkResults(results);
		}
	}
	catch (ATN::Exception e) {}
}
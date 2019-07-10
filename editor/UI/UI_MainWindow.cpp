#include "UI_MainWindow.h"

#include "ATN/ATN_Entry.h"
#include "ATN/ATN_List.h"
#include "ATN/ATN_Manager.h"

#include <typeinfo>

#include <QFileDialog>
#include <QVBoxLayout>

#include "UI_MainWindowSearchResult.h"
#include "UI_NetworkContainer.h"

#include <QListWidgetItem>
#include <QToolTip>

UI_MainWindow::UI_MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Disable close button on search results tab
	ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
}

void UI_MainWindow::setNetworkResults(std::vector<ATN::Network*> results)
{
	ui.listSearchResults->clear();

	for (ATN::Network *net : results)
	{
		UI_MainWindowSearchResult *res = new UI_MainWindowSearchResult();

		res->ui.textName->setText(QString::fromStdString(net->name()));
		res->ui.textUniqueID->setText(QString::fromStdString(std::to_string(net->id())));

		connect(res->ui.buttonOpenNetwork, SIGNAL(clicked()), this, SLOT(openNetworkButton()));

		QListWidgetItem *item = new QListWidgetItem();
		item->setSizeHint(res->size());

		ui.listSearchResults->addItem(item);
		ui.listSearchResults->setItemWidget(item, res);
	}
}

void UI_MainWindow::createNetworkResourceTab(ATN::Network &el)
{
	UI_NetworkContainer *tab = new UI_NetworkContainer;

	tab->initializeFromID(el.id());

	connect(tab, SIGNAL(openNetworkRequest(int)), this, SLOT(receiveOpenNetworkRequest(int)));
	connect(tab, SIGNAL(findTransitionsRequest(int)), this, SLOT(receiveTransitionsRequest(int)));

	ui.tabWidget->addTab(tab, tr("Network ") + QString::fromStdString(std::to_string(el.id())));

	ui.tabWidget->setCurrentIndex(ui.tabWidget->count() - 1);
}

void UI_MainWindow::closeEvent(QCloseEvent *event)
{
	m_hashWindow.close();
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

void UI_MainWindow::openHashTool()
{
	m_hashWindow.show();
}

void UI_MainWindow::searchName(const QString &str)
{
	std::vector<ATN::Network*> netResults;

	// Focus on search results
	ui.tabWidget->setCurrentIndex(0);

	// Too laggy and often irrelevant to search on first character
	if (str.length() <= 1)
	{
		setNetworkResults(netResults);
		return;
	}

	std::vector<std::pair<ATN::Entry*, ATN::List<ATN::Entry>*>> res = ATN::Manager::searchName(str.toStdString());

	for (std::pair<ATN::Entry*, ATN::List<ATN::Entry>*> &pair : res)
	{
		if (typeid(*pair.first) == typeid(ATN::Network))
		{
			netResults.push_back((ATN::Network*)pair.first);
		}
	}

	setNetworkResults(netResults);
}

void UI_MainWindow::searchID(const QString &str)
{
	// Clear current results
	setNetworkResults(std::vector<ATN::Network*>());

	// Focus on search results
	ui.tabWidget->setCurrentIndex(0);

	try
	{
		int id = std::atoi(str.toStdString().c_str());

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

void UI_MainWindow::openNetworkButton()
{
	UI_MainWindowSearchResult *res = (UI_MainWindowSearchResult*)sender()->parent();

	ATN::Network *net = (ATN::Network*)&ATN::Manager::findByID(std::stoi(res->ui.textUniqueID->text().toStdString()));

	for (size_t i = 1; i < ui.tabWidget->tabBar()->count(); i++)
	{
		UI_NetworkContainer* tabItem = (UI_NetworkContainer*)ui.tabWidget->widget(i);

		// Focus the existing tab if network is already opened
		if (tabItem->network().id() == net->id())
		{
			ui.tabWidget->setCurrentIndex(i);
			return;
		}
	}

	createNetworkResourceTab(*net);
}

void UI_MainWindow::closeTab(int tab)
{
	QWidget* tabItem = ui.tabWidget->widget(tab);
	// Removes the tab at position index from this stack of widgets.
	// The page widget itself is not deleted.
	ui.tabWidget->removeTab(tab);

	delete tabItem;
	tabItem = nullptr;
}

void UI_MainWindow::receiveOpenNetworkRequest(int id)
{
	for (size_t i = 1; i < ui.tabWidget->tabBar()->count(); i++)
	{
		UI_NetworkContainer* tabItem = (UI_NetworkContainer*)ui.tabWidget->widget(i);

		// Focus the existing tab if network is already opened
		if (tabItem->network().id() == id)
		{
			ui.tabWidget->setCurrentIndex(i);
			return;
		}
	}

	ATN::Network *net = (ATN::Network*)&ATN::Manager::findByID(id);

	createNetworkResourceTab(*net);
}

void UI_MainWindow::receiveTransitionsRequest(int id)
{
	blockSignals(true);

	ui.textName->setText("");
	ui.textUniqueID->setText("");

	blockSignals(false);

	ATN::Network *transitionNetwork = (ATN::Network*)&ATN::Manager::findByID(id);

	// Clear current results
	setNetworkResults(std::vector<ATN::Network*>());

	// Focus on search results
	ui.tabWidget->setCurrentIndex(0);

	std::vector<ATN::Network*> netResults;

	for (ATN::Network *net : ATN::Manager::getNetworks())
	{
		for (ATN::State *state : net->states())
		{
			if (state->networkTransition() == transitionNetwork)
			{
				netResults.push_back(net);
				break;
			}
		}
	}

	setNetworkResults(netResults);
}

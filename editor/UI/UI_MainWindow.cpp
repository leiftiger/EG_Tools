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

void UI_MainWindow::setNetworkResults(std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>> results)
{
	ui.listSearchResults->clear();

	for (std::pair<ATN::Network*, ATN::List<ATN::Entry>*> &pair : results)
	{
		ATN::Network *net = pair.first;
		ATN::List<ATN::Entry> *netList = pair.second;

		UI_MainWindowSearchResult *res = new UI_MainWindowSearchResult();

		res->ui.textName->setText(QString::fromStdString(net->name()));
		res->ui.textUniqueID->setText(QString::fromStdString(std::to_string(net->id())));

		std::string name = netList->name();

		int nameStart = name.find_last_of('/') + 1;

		res->ui.textATNList->setText(QString::fromStdString(name.substr(nameStart, name.length()-nameStart)));

		connect(res->ui.buttonCopyNetwork, SIGNAL(clicked()), this, SLOT(copyNetworkButton()));
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

	if (ATN::Manager::isMultiDefined(el))
		tab->setReadOnly(true);

	tab->initializeFromID(el.id());

	connect(tab, SIGNAL(openNetworkRequest(int)), this, SLOT(receiveOpenNetworkRequest(int)));
	connect(tab, SIGNAL(closeNetworkRequest(int)), this, SLOT(receiveCloseNetworkRequest(int)));
	connect(tab, SIGNAL(findTransitionsRequest(int)), this, SLOT(receiveTransitionsRequest(int)));
	connect(tab, SIGNAL(repopulateNeighbors(int)), this, SLOT(receiveRepopulationRequest(int)));

	ui.tabWidget->addTab(tab, tr("Network ") + QString::fromStdString(std::to_string(el.id())));

	ui.tabWidget->setCurrentIndex(ui.tabWidget->count() - 1);
}

void UI_MainWindow::closeEvent(QCloseEvent *event)
{
	m_hashWindow.close();
	m_errorWindow.close();
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

void UI_MainWindow::newFile()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save name"), "", tr("ATN files (*.ros)"));

	// Aborted
	if (file.size() == 0)
		return;

	ATN::List<ATN::Entry> *list = new ATN::List<ATN::Entry>(file.toStdString());

	m_atnFiles.push_back(file.toStdString());

	ATN::Manager::addList(list);

	reloadFileList();
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

void UI_MainWindow::saveFiles()
{
	try
	{
		ATN::Manager::saveToFiles();
	}
	catch(ATN::Exception &e)
	{
		m_errorWindow.setErrorMessage(e.what());
		m_errorWindow.setWindowModality(Qt::WindowModality::ApplicationModal);
		m_errorWindow.show();
	}
}

void UI_MainWindow::newNetwork()
{
	int index = ui.comboATNList->currentIndex();

	ATN::List<ATN::Entry> *netList = ATN::Manager::lists()[index];

	ATN::Network *net = new ATN::Network();
	net->setName("New network");
	net->setID(ATN::Manager::maxID() + 1);

	netList->add(*net);
	ATN::Manager::addEntry(*net);

	createNetworkResourceTab(*net);
}

void UI_MainWindow::openHashTool()
{
	m_hashWindow.show();
}

void UI_MainWindow::searchName(const QString &str)
{
	std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>> netResults;

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
			netResults.push_back(std::make_pair((ATN::Network*)pair.first, pair.second));
		}
	}

	setNetworkResults(netResults);
}

void UI_MainWindow::searchID(const QString &str)
{
	// Clear current results
	setNetworkResults(std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>>());

	// Focus on search results
	ui.tabWidget->setCurrentIndex(0);

	try
	{
		int id = std::atoi(str.toStdString().c_str());

		ATN::List<ATN::Entry> *netList;

		ATN::Entry &el = ATN::Manager::findByID(id, netList);

		if (typeid(el) == typeid(ATN::Network))
		{
			std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>> results;

			results.push_back(std::make_pair((ATN::Network*)&el, netList));

			setNetworkResults(results);
		}
	}
	catch (ATN::Exception e) {}
}

void UI_MainWindow::copyNetworkButton()
{
	UI_MainWindowSearchResult *res = (UI_MainWindowSearchResult*)sender()->parent();

	ATN::Network *net = (ATN::Network*)&ATN::Manager::findByID(std::stoi(res->ui.textUniqueID->text().toStdString()));

	std::string listName = res->ui.textATNList->text().toStdString();

	ATN::List<ATN::Entry> *netList = nullptr;

	for (ATN::List<ATN::Entry> *list : ATN::Manager::lists())
	{
		if (list->name().find(listName) > 0)
		{
			netList = list;
			break;
		}
	}

	if (netList == nullptr)
	{
		throw ATN::Exception("Couldn't find a loaded ATN list matching \"%s\"", listName);
	}

	std::uint32_t id = ATN::Manager::maxID()+1;

	ATN::Network *netCopy = new ATN::Network();
	netCopy->setID(id++);
	netCopy->setName(std::string("Copy of ") + net->name());

	netList->add(*netCopy);
	ATN::Manager::addEntry(*netCopy);

	std::vector<ATN::State*> copyStates;

	std::unordered_map<const ATN::State*, int> oldStatesToIndex;


	for (ATN::Resource *resource : net->resources())
	{
		ATN::Resource *resourceCopy = new ATN::Resource(*resource);

		netCopy->add(*resourceCopy);
	}

	for (ATN::Parameter *parameter : net->parameters())
	{
		ATN::Parameter *parameterCopy = new ATN::Parameter(*parameter);

		netCopy->add(*parameterCopy);
	}

	for (int stateIndex = 0; stateIndex < net->states().size(); stateIndex++)
	{
		ATN::State *state = net->states()[stateIndex];

		ATN::State *stateCopy = new ATN::State();
		stateCopy->setID(id++);
		stateCopy->setName(state->name());

		stateCopy->copyNetworkTransition(state);

		netCopy->add(*stateCopy);

		netList->add(*stateCopy);
		ATN::Manager::addEntry(*stateCopy);

		copyStates.push_back(stateCopy);
		oldStatesToIndex[state] = stateIndex;
	}

	// Keep invalid pointers as invalid as they were in the original
	copyStates.push_back(nullptr);
	oldStatesToIndex[nullptr] = copyStates.size() - 1;

	for (ATN::Thread *thread : net->threads())
	{
		ATN::Thread *threadCopy = new ATN::Thread();
		threadCopy->setID(id++);
		threadCopy->setName(thread->name());
		threadCopy->setNetwork(*netCopy);

		threadCopy->setState(copyStates[oldStatesToIndex[thread->state()]]);

		netCopy->add(*threadCopy);

		netList->add(*threadCopy);
		ATN::Manager::addEntry(*threadCopy);
	}

	for (ATN::State *state : net->states())
	{
		ATN::State *stateCopy = copyStates[oldStatesToIndex[state]];

		for (ATN::Transition *transition : state->transitions())
		{
			ATN::Transition *transitionCopy = new ATN::Transition();
			transitionCopy->setID(id++);

			transitionCopy->copyFrom(transition);

			transitionCopy->setState(copyStates[oldStatesToIndex[transition->state()]]);

			stateCopy->add(*transitionCopy);

			netList->add(*transitionCopy);
			ATN::Manager::addEntry(*transitionCopy);
		}
	}

	createNetworkResourceTab(*netCopy);
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

	if (ATN::Manager::isMultiDefined(*net))
	{
		QMessageBox msg;

		msg.setWindowFlags(Qt::Dialog | Qt::Desktop);
		msg.setIcon(QMessageBox::Icon::Information);

		msg.setWindowTitle(tr(" "));
		msg.setText(tr("<span style=\"font-size:12pt;\"><b>Network is read-only</b></span>"));

		msg.setInformativeText(tr("This network is defined in multiple files, thus it is set to read-only due to program limitations.") +
			tr("\n\nIf you want to edit it, you must load only \"") + res->ui.textATNList->text() +
			tr("\" and \"ATNResources.ros\".\n\nIf you do edit this network, you must apply the same changes in the other files or risk crashing the game."));

		msg.setStandardButtons(QMessageBox::Ok);
		msg.setDefaultButton(QMessageBox::Ok);

		QApplication::beep();
		int ret = msg.exec();
	}
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

	if (ATN::Manager::isMultiDefined(*net))
	{
		QMessageBox msg;

		msg.setWindowFlags(Qt::Dialog | Qt::Desktop);
		msg.setIcon(QMessageBox::Icon::Information);

		msg.setWindowTitle(tr(" "));
		msg.setText(tr("<span style=\"font-size:12pt;\"><b>Network is read-only</b></span>"));

		msg.setInformativeText(tr("This network is defined in multiple files, thus it is set to read-only due to program limitations.") +
			tr("\n\nIf you want to edit it, you must load only one of the files it is defined in ") +
			tr("and \"ATNResources.ros\".\n\nIf you do edit this network, you must apply the same changes in the other files or risk crashing the game."));

		msg.setStandardButtons(QMessageBox::Ok);
		msg.setDefaultButton(QMessageBox::Ok);

		QApplication::beep();
		int ret = msg.exec();
	}
}

void UI_MainWindow::receiveCloseNetworkRequest(int id)
{
	for (size_t i = 1; i < ui.tabWidget->tabBar()->count(); i++)
	{
		UI_NetworkContainer* tabItem = (UI_NetworkContainer*)ui.tabWidget->widget(i);


		if (tabItem->network().id() == id)
		{
			ui.tabWidget->removeTab(i);
			break;
		}
	}

	// Remove the network from any open search results
	for (size_t i = 0; i < ui.listSearchResults->count(); i++)
	{
		QListWidgetItem *item = ui.listSearchResults->item(i);

		UI_MainWindowSearchResult* res = (UI_MainWindowSearchResult*)ui.listSearchResults->itemWidget(item);

		if (std::stoi(res->ui.textUniqueID->text().toStdString()) == id)
		{
			ui.listSearchResults->takeItem(i);

			res->deleteLater();

			i--;
		}
	}
}

void UI_MainWindow::receiveTransitionsRequest(int id)
{
	blockSignals(true);

	ui.textName->setText("");
	ui.textUniqueID->setText("");

	blockSignals(false);

	ATN::Network *transitionNetwork = (ATN::Network*)&ATN::Manager::findByID(id);

	// Clear current results
	setNetworkResults(std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>>());

	// Focus on search results
	ui.tabWidget->setCurrentIndex(0);

	std::vector<std::pair<ATN::Network*, ATN::List<ATN::Entry>*>> netResults;

	for (ATN::Network *net : ATN::Manager::getNetworks())
	{
		for (ATN::State *state : net->states())
		{
			if (state->networkTransition() == transitionNetwork)
			{
				ATN::List<ATN::Entry> *netList;

				ATN::Manager::findByID(net->id(), netList);

				netResults.push_back(std::make_pair(net, netList));
				break;
			}
		}
	}

	setNetworkResults(netResults);
}

void UI_MainWindow::receiveRepopulationRequest(int id)
{
	for (size_t i = 1; i < ui.tabWidget->tabBar()->count(); i++)
	{
		UI_NetworkContainer* tabItem = (UI_NetworkContainer*)ui.tabWidget->widget(i);

		for (ATN::State *state : tabItem->network().states())
		{
			if (state->networkTransition() != nullptr && state->networkTransition()->id() == id)
			{
				tabItem->repopulateArguments(false);
				break;
			}
		}
	}
}

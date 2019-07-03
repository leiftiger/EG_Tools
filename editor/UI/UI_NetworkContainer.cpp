#include "UI_NetworkContainer.h"

void UI_NetworkContainer::initializeArgumentLists()
{
	m_effectList.clear();
	m_perceptList.clear();

	m_effectList.append("-1: Do nothing (preferred)");

	for (const ATN::Effect *e : ATN::Manager::getEffects())
	{
		m_effectList.append(QString::fromStdString(std::to_string(e->id()) + ": " + e->name() + " (" + std::string(e->typeName()).substr(strlen("TATNEffect")) + ")"));
	}

	for (const ATN::Percept *e : ATN::Manager::getPercepts())
	{
		m_perceptList.append(QString::fromStdString(std::to_string(e->id()) + ": " + e->name() + " (" + std::string(e->typeName()).substr(strlen("TATNPercept")) + ")"));
	}

	ui.comboTransitionEffect->clear();
	ui.comboTransitionPercept->clear();

	ui.comboTransitionEffect->addItems(m_effectList);
	ui.comboTransitionPercept->addItems(m_perceptList);
}

void UI_NetworkContainer::initializeThreads()
{
	for (ATN::Thread *t : m_network->threads())
	{
		UI_NetworkThread *ut = new UI_NetworkThread(ui.listNetworkThreads);

		ut->ui.textThreadName->setText(QString::fromStdString(t->name()));

		ut->m_thread = t;

		connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(threadMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(threadMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(threadRemove()));

		m_threads.push_back(ut);
	}

	layoutSortables(m_threads, ui.listNetworkThreads);
}

void UI_NetworkContainer::threadCreate()
{
	ATN::Thread *t = new ATN::Thread();

	t->setID(ATN::Manager::maxID()+1);
	t->setName(std::string("Thread ") + std::to_string(m_threads.size()+1));
	t->setNetwork(*m_network);

	// Add this object to the same ATN list the network belongs to
	ATN::List<ATN::Entry> *outList;

	ATN::Manager::findByID(m_network->id(), outList);

	outList->add(*(ATN::Entry*)t);
	ATN::Manager::addEntry(*(ATN::Entry*)t);

	m_network->add(*t);

	UI_NetworkThread *ut = new UI_NetworkThread(ui.listNetworkThreads);

	ut->ui.textThreadName->setText(QString::fromStdString(t->name()));

	ut->m_thread = t;

	connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(threadMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(threadMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(threadRemove()));

	m_threads.push_back(ut);

	ut->show();

	layoutSortables(m_threads, ui.listNetworkThreads);

	// Scroll to bottom in case we have many threads
	QScrollArea *scroller = (QScrollArea*)ui.listNetworkThreads->parent()->parent();

	scroller->verticalScrollBar()->setSliderPosition(scroller->verticalScrollBar()->maximum());
}

void UI_NetworkContainer::threadMoveUp()
{
	UI_NetworkThread *ut = (UI_NetworkThread*)sender()->parent();

	itemMove(m_threads, ut, true);
	layoutSortables(m_threads, ui.listNetworkThreads);

	m_network->moveUp(*ut->m_thread);
}

void UI_NetworkContainer::threadMoveDown()
{
	UI_NetworkThread *ut = (UI_NetworkThread*)sender()->parent();

	itemMove(m_threads, ut, false);
	layoutSortables(m_threads, ui.listNetworkThreads);

	m_network->moveDown(*ut->m_thread);
}

void UI_NetworkContainer::threadRemove()
{
	UI_NetworkThread *ut = (UI_NetworkThread*)sender()->parent();

	itemRemove(m_threads, ut);
	layoutSortables(m_threads, ui.listNetworkThreads);

	m_network->remove(*ut->m_thread);

	deleteATN(ut->m_thread);

	ut->deleteLater();
}

void UI_NetworkContainer::stateCreate()
{

}

void UI_NetworkContainer::stateMoveUp()
{

}

void UI_NetworkContainer::stateMoveDown()
{

}

void UI_NetworkContainer::stateRemove()
{

}

void UI_NetworkContainer::resourceCreate()
{

}

void UI_NetworkContainer::resourceMoveUp()
{

}

void UI_NetworkContainer::resourceMoveDown()
{

}

void UI_NetworkContainer::resourceRemove()
{

}

void UI_NetworkContainer::variableCreate()
{

}

void UI_NetworkContainer::variableMoveUp()
{

}

void UI_NetworkContainer::variableMoveDown()
{

}

void UI_NetworkContainer::variableRemove()
{

}

void UI_NetworkContainer::setNetworkName(const QString &name)
{
	// Because of initial loading, this may be called unnecessarily
	if (m_network->name() == name.toStdString())
		return;

	m_network->setName(name.toStdString());
}

void UI_NetworkContainer::setTransitionName(const QString &name)
{

}

void UI_NetworkContainer::setTransitionPercept(int index)
{

}

void UI_NetworkContainer::setTransitionEffect(int index)
{

}

void UI_NetworkContainer::deleteNetwork()
{
	// TODO: Prompt user to confirm this very dangerous action
	// TODO: Check if any states not owned by this network transitions to it and force user to remove those references first
}

void UI_NetworkContainer::deleteTransition()
{

}


void UI_NetworkContainer::initializeResources()
{

}

void UI_NetworkContainer::initializeVariables()
{

}

UI_NetworkContainer::UI_NetworkContainer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// Only enabled when highlighting a transition
	ui.frameTransition->setEnabled(false);

	initializeArgumentLists();
}

UI_NetworkContainer::~UI_NetworkContainer()
{

}

void UI_NetworkContainer::initializeFromID(std::int32_t id)
{
	m_network = (ATN::Network*)&ATN::Manager::findByID(id);

	ui.textNetworkName->setText(QString::fromStdString(m_network->name()));
	ui.textNetworkID->setText(QString::fromStdString(std::to_string(m_network->id())));

	initializeThreads();
	initializeResources();
	initializeVariables();
}

const ATN::Network &UI_NetworkContainer::network()
{
	return *m_network;
}
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
	int x = 0, y = 0;

	UI_NetworkThread *ut = nullptr;

	for (ATN::Thread *t : m_network->threads())
	{
		bool bIsFirstItem = (ut == nullptr);

		ut = new UI_NetworkThread(ui.listNetworkThreads);

		ut->ui.textThreadName->setText(QString::fromStdString(t->name()));

		ut->m_thread = t;

		connect(ut->ui.buttonSortUp, SIGNAL(pressed()), this, SLOT(threadMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(pressed()), this, SLOT(threadMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(pressed()), this, SLOT(threadRemove()));

		if (bIsFirstItem)
			ut->ui.buttonSortUp->setEnabled(false);

		ut->move(0, y);

		y += ut->size().height();

		m_threads.push_back(ut);
	}

	ui.listNetworkThreads->setMinimumHeight(y);

	// Last item can't be moved further down
	ut->ui.buttonSortDown->setEnabled(false);
}

void UI_NetworkContainer::threadMoveUp()
{

}

void UI_NetworkContainer::threadMoveDown()
{

}

void UI_NetworkContainer::threadRemove()
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
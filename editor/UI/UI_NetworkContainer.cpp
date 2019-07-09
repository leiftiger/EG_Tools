#include "UI_NetworkContainer.h"

void UI_NetworkContainer::initializeArgumentLists()
{
	m_effectList.clear();
	m_perceptList.clear();
	m_resourceTypes.clear();

	m_effectList.append("-1: Do nothing (preferred)");

	for (const ATN::Effect *e : ATN::Manager::getEffects())
	{
		m_effectList.append(QString::fromStdString(std::to_string(e->id()) + ": " + e->name() + " (" + std::string(e->typeName()).substr(strlen("TATNEffect")) + ")"));
	}

	for (const ATN::Percept *e : ATN::Manager::getPercepts())
	{
		m_perceptList.append(QString::fromStdString(std::to_string(e->id()) + ": " + e->name() + " (" + std::string(e->typeName()).substr(strlen("TATNPercept")) + ")"));
	}

	for (const char *name : ATN::ResourceType::_names())
	{
		m_resourceTypes.append(tr(name));
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
	ATN::State *s = new ATN::State();

	s->setID(ATN::Manager::maxID() + 1);
	s->setName(std::string("State ") + std::to_string(m_states.size() + 1));

	// Add this object to the same ATN list the network belongs to
	ATN::List<ATN::Entry> *outList;

	ATN::Manager::findByID(m_network->id(), outList);

	outList->add(*(ATN::Entry*)s);
	ATN::Manager::addEntry(*(ATN::Entry*)s);

	m_network->add(*s);

	UI_NetworkState *ut = new UI_NetworkState(ui.frameStates);

	connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(stateMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(stateMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(stateRemove()));

	ut->initialize(s, m_network);
	ut->show();

	m_states.push_back(ut);

	layoutStates();
}

void UI_NetworkContainer::stateMoveUp()
{
	UI_NetworkState *ut = (UI_NetworkState*)sender()->parent()->parent();

	itemMove(m_states, ut, true);

	layoutStates();

	m_network->moveUp(*ut->m_state);
}

void UI_NetworkContainer::stateMoveDown()
{
	UI_NetworkState *ut = (UI_NetworkState*)sender()->parent()->parent();

	itemMove(m_states, ut, false);

	layoutStates();

	m_network->moveDown(*ut->m_state);
}

void UI_NetworkContainer::stateRemove()
{
	UI_NetworkState *ut = (UI_NetworkState*)sender()->parent()->parent();

	itemRemove(m_states, ut);

	layoutStates();

	m_network->remove(*ut->m_state);

	deleteATN(ut->m_state);

	ut->deleteLater();
}

void UI_NetworkContainer::resourceCreate()
{
	UI_NetworkResource *ut = new UI_NetworkResource(ui.listNetworkResources);

	ATN::Resource *r = new ATN::Resource(ATN::ResourceType::Number, std::string("Resource ") + std::to_string(m_resources.size() + 1), true);

	ut->m_resource = r;

	m_network->add(*r);

	// Remove placeholder option
	ut->ui.resourceType->clear();

	ut->ui.resourceType->addItems(m_resourceTypes);
	ut->ui.resourceType->setCurrentIndex(r->m_type._to_index());

	ut->ui.resourceDesc->setText(QString::fromStdString(r->m_desc));

	ut->ui.resourceIsParameter->setChecked(!r->m_internalResource);

	connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(resourceMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(resourceMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(resourceRemove()));

	m_resources.push_back(ut);

	ut->show();

	layoutSortables(m_resources, ui.listNetworkResources);

	// Scroll to bottom in case we have many threads
	QScrollArea *scroller = (QScrollArea*)ui.listNetworkResources->parent()->parent();

	scroller->verticalScrollBar()->setSliderPosition(scroller->verticalScrollBar()->maximum());
}

void UI_NetworkContainer::resourceMoveUp()
{
	UI_NetworkResource *ut = (UI_NetworkResource*)sender()->parent();

	itemMove(m_resources, ut, true);
	layoutSortables(m_resources, ui.listNetworkResources);

	m_network->moveUp(*ut->m_resource);
}

void UI_NetworkContainer::resourceMoveDown()
{
	UI_NetworkResource *ut = (UI_NetworkResource*)sender()->parent();

	itemMove(m_resources, ut, false);
	layoutSortables(m_resources, ui.listNetworkResources);

	m_network->moveDown(*ut->m_resource);
}

void UI_NetworkContainer::resourceRemove()
{
	UI_NetworkResource *ut = (UI_NetworkResource*)sender()->parent();

	itemRemove(m_resources, ut);
	layoutSortables(m_resources, ui.listNetworkResources);

	m_network->remove(*ut->m_resource);

	delete ut->m_resource;

	ut->deleteLater();
}

void UI_NetworkContainer::variableCreate()
{
	UI_NetworkVariable *ut = new UI_NetworkVariable(ui.listNetworkVariables);

	ATN::Parameter *p = new ATN::Parameter("Integer", 0, std::string("Variable ") + std::to_string(m_variables.size() + 1));

	ut->m_variable = p;

	// Remove placeholder option
	ut->ui.variableType->clear();

	ut->ui.variableType->addItems(m_variableTypes);
	ut->ui.variableType->setCurrentIndex(ut->ui.variableType->findText(QString::fromStdString(p->m_type)));

	ut->ui.variableDesc->setText(QString::fromStdString(p->m_desc));

	// Remove placeholder option
	ut->ui.variableValue->clear();

	ut->loadTranslations();

	ut->ui.variableValue->setCurrentText(QString::fromStdString(p->translateValue(p->m_defaultValue)));

	connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(variableMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(variableMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(variableRemove()));

	m_variables.push_back(ut);

	ut->show();

	layoutSortables(m_variables, ui.listNetworkVariables);

	// Scroll to bottom in case we have many threads
	QScrollArea *scroller = (QScrollArea*)ui.listNetworkVariables->parent()->parent();

	scroller->verticalScrollBar()->setSliderPosition(scroller->verticalScrollBar()->maximum());
}

void UI_NetworkContainer::variableMoveUp()
{
	UI_NetworkVariable *ut = (UI_NetworkVariable*)sender()->parent();

	itemMove(m_variables, ut, true);
	layoutSortables(m_variables, ui.listNetworkVariables);

	m_network->moveUp(*ut->m_variable);
}

void UI_NetworkContainer::variableMoveDown()
{
	UI_NetworkVariable *ut = (UI_NetworkVariable*)sender()->parent();

	itemMove(m_variables, ut, false);
	layoutSortables(m_variables, ui.listNetworkVariables);

	m_network->moveDown(*ut->m_variable);
}

void UI_NetworkContainer::variableRemove()
{
	UI_NetworkVariable *ut = (UI_NetworkVariable*)sender()->parent();

	itemRemove(m_variables, ut);
	layoutSortables(m_variables, ui.listNetworkVariables);

	m_network->remove(*ut->m_variable);

	delete ut->m_variable;

	ut->deleteLater();
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
	for (ATN::Resource *r : m_network->resources())
	{
		UI_NetworkResource *ut = new UI_NetworkResource(ui.listNetworkResources);

		ut->m_resource = r;

		// Remove placeholder option
		ut->ui.resourceType->clear();

		ut->ui.resourceType->addItems(m_resourceTypes);
		ut->ui.resourceType->setCurrentIndex(r->m_type._to_index());

		ut->ui.resourceDesc->setText(QString::fromStdString(r->m_desc));

		ut->ui.resourceIsParameter->setChecked(!r->m_internalResource);

		connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(resourceMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(resourceMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(resourceRemove()));

		m_resources.push_back(ut);
	}

	layoutSortables(m_resources, ui.listNetworkResources);
}

void UI_NetworkContainer::initializeVariables()
{
	for (ATN::Parameter *p : m_network->parameters())
	{
		UI_NetworkVariable *ut = new UI_NetworkVariable(ui.listNetworkVariables);

		ut->m_variable = p;

		// Remove placeholder option
		ut->ui.variableType->clear();

		ut->ui.variableType->addItems(m_variableTypes);
		ut->ui.variableType->setCurrentIndex(ut->ui.variableType->findText(QString::fromStdString(p->m_type)));

		ut->ui.variableDesc->setText(QString::fromStdString(p->m_desc));

		// Remove placeholder option
		ut->ui.variableValue->clear();

		ut->loadTranslations();

		ut->ui.variableValue->setCurrentText(QString::fromStdString(p->translateValue(p->m_defaultValue)));

		connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(variableMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(variableMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(variableRemove()));

		m_variables.push_back(ut);
	}

	layoutSortables(m_variables, ui.listNetworkVariables);
}

void UI_NetworkContainer::layoutStates()
{
	int x = 0, y = 0;

	for (size_t i = 0; i < m_states.size(); i++)
	{
		UI_NetworkState *ut = m_states[i];

		// Disable first and last sort buttons
		ut->ui.buttonSortUp->setEnabled(i != 0);
		ut->ui.buttonSortDown->setEnabled(i != m_states.size() - 1);

		ut->move(x, 0);

		x += ut->width() + STATE_MARGIN;
	}

	ui.frameStates->adjustSize();

	ui.frameStates->setFixedWidth(ui.frameStates->width() + STATE_MARGIN);

	// Adjust scroller to recognize the total area
	ui.networkContents->setMinimumWidth(std::max(m_networkContentsMinimumSize.width(), ui.frameStates->width() + ui.frameStates->x()));
	ui.networkContents->setMinimumHeight(std::max(m_networkContentsMinimumSize.height(), ui.frameStates->height() + ui.frameStates->y() + NetworkContainerProxy::CONNECTOR_HEIGHT_OFFSET*4));

	m_proxy.setUpperHeight(ui.frameStates->y());
	m_proxy.setLowerHeight(ui.frameStates->y() + ui.frameStates->height() + 24); // TODO: Why doesn't the height go to +24 automatically?
}

void UI_NetworkContainer::initializeStates()
{
	std::unordered_map<const ATN::State*, UI_NetworkState*> uiStates;

	for (ATN::State *s : m_network->states())
	{
		UI_NetworkState *ut = new UI_NetworkState(ui.frameStates);

		ut->initialize(s, m_network);

		connect(ut->ui.buttonSortUp, SIGNAL(released()), this, SLOT(stateMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(released()), this, SLOT(stateMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(released()), this, SLOT(stateRemove()));

		m_states.push_back(ut);

		uiStates[s] = ut;
	}

	for (UI_NetworkThread *uiThread : m_threads)
	{
		const ATN::State *state = &uiThread->m_thread->state();

		if (state != nullptr)
		{
			UI_NetworkState *uiState = uiStates[state];

			UI_Connector *uiConnector = new UI_Connector(ui.networkContents);

			uiConnector->setNetwork(&m_proxy);

			uiConnector->setStart(uiThread->ui.connector);
			uiConnector->setEnd(uiState->ui.connectorIn);

			uiThread->ui.connector->setConnector(uiConnector);
		}
	}

	for (UI_NetworkState *uiStateFrom : m_states)
	{
		for (ATN::Transition *t : uiStateFrom->m_state->transitions())
		{
			UI_NetworkTransition *uiTransition = new UI_NetworkTransition(uiStateFrom);

			uiTransition->m_transition = t;

			UI_NetworkState *uiStateTo = uiStates[&t->state()];

			UI_Connector *uiConnector = new UI_Connector(ui.networkContents);

			uiConnector->setNetwork(&m_proxy);

			uiConnector->setStart(uiTransition->m_connector);
			uiConnector->setEnd(uiStateTo->ui.connectorIn);

			uiTransition->m_connector->setConnector(uiConnector);

			uiStateFrom->ui.connectorOut->addTransition(uiTransition);

			uiStateFrom->adjustSize();
		}
	}

	layoutStates();
}

UI_NetworkContainer::UI_NetworkContainer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_networkContentsMinimumSize = ui.networkContents->minimumSize();

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
	initializeStates();
}

const ATN::Network &UI_NetworkContainer::network()
{
	return *m_network;
}

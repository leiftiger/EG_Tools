#include "UI_NetworkContainer.h"

void UI_NetworkContainer::initializeArgumentLists()
{
	m_effectList.clear();
	m_perceptList.clear();
	m_resourceTypes.clear();

	m_effectList.append("0: No Effect (Preferred)");

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

		connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(threadMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(threadMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(threadRemove()));

		connect(ut->ui.connector, SIGNAL(createNewConnector()), this, SLOT(createNewConnector()));

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

	connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(threadMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(threadMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(threadRemove()));

	connect(ut->ui.connector, SIGNAL(createNewConnector()), this, SLOT(createNewConnector()));

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

	connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(stateMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(stateMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(stateRemove()));

	connect(ut, SIGNAL(openNetworkRequest(int)), this, SLOT(receiveOpenNetworkRequest(int)));
	connect(ut, SIGNAL(requestLayout()), this, SLOT(receiveStateLayoutRequest()));
	connect(ut, SIGNAL(requestNewTransition()), this, SLOT(createNewTransition()));

	connect(ut->ui.connectorOut->transitionConnector(), SIGNAL(createNewConnector()), this, SLOT(createNewConnector()));

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

	connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(resourceMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(resourceMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(resourceRemove()));

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

	ut->blockSignals(true);

	// Remove placeholder option
	ut->ui.variableType->clear();

	ut->ui.variableType->addItems(m_variableTypes);
	ut->ui.variableType->setCurrentIndex(ut->ui.variableType->findText(QString::fromStdString(p->m_type)));

	ut->ui.variableDesc->setText(QString::fromStdString(p->m_desc));

	ut->loadTranslations();

	ut->ui.variableValue->setCurrentText(QString::fromStdString(p->translateValue(p->m_defaultValue)));

	ut->blockSignals(false);

	connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(variableMoveUp()));
	connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(variableMoveDown()));
	connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(variableRemove()));
	connect(ut, SIGNAL(changeType(QString)), this, SLOT(variableTypeChange(QString)));
	connect(ut, SIGNAL(repopulateArguments()), this, SLOT(repopulateArguments()));

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

void UI_NetworkContainer::variableTypeChange(const QString &qType)
{
	UI_NetworkVariable *uiVariable = (UI_NetworkVariable*)sender();

	std::string newType = qType.toStdString();

	std::string oldType = uiVariable->m_variable->m_type;

	// No need to make any changes
	if (oldType == newType)
		return;

	// TODO: Show dialog to user that this will reset all arguments referring to this index
	// (including in external networks) and allow them to abort if they didn't want to this

	std::int64_t index = -1;

	for (size_t i = 0; i < m_variables.size(); i++)
	{
		if (m_variables[i] == uiVariable)
		{
			index = (std::int64_t)i;
			break;
		}
	}

	uiVariable->m_variable->m_type = newType;

	for (UI_NetworkState *uiState : m_states)
	{
		for (ATN::ParameterMarshall *paramMarshall : uiState->m_state->parameterMarshalls())
		{
			// Any arguments that pointed to this index are reset to constant 0
			// so that if the variable is now incompatible with that argument, it won't create a broken state
			// (user has to find all uses themselves)
			paramMarshall->resetConstant(index);
		}

		for (ATN::Transition *transition : uiState->m_state->transitions())
		{
			for (ATN::ParameterMarshall *paramMarshall : transition->effectParameterMarshalls())
			{
				paramMarshall->resetConstant(index);
			}
			for (ATN::ParameterMarshall *paramMarshall : transition->perceptParameterMarshalls())
			{
				paramMarshall->resetConstant(index);
			}
		}
	}

	for (ATN::Network *net : ATN::Manager::getNetworks())
	{
		for (ATN::State *state : net->states())
		{
			if (state->networkTransition() != nullptr && state->networkTransition() == m_network)
			{
				// Here we reset the argument that will be passed to our index
				int numMarshalls = state->parameterMarshalls().size();

				if (index < numMarshalls)
					state->parameterMarshalls()[index]->resetConstant();
			}
		}
	}

	uiVariable->loadTranslations();

	repopulateArguments();
}

void UI_NetworkContainer::setNetworkName(const QString &name)
{
	ui.textNetworkName->setInputError(false);

	// Because of initial loading, this may be called unnecessarily
	if (m_network->name() == name.toStdString())
		return;

	try
	{
		ATN::Network *net = (ATN::Network*)&ATN::Manager::findByName(name.toStdString());

		if (net != m_network)
		{
			ui.textNetworkName->setInputError(true);

			QToolTip::showText(ui.textNetworkName->mapToGlobal(ui.textNetworkName->pos()), tr("This name already exists, network name must be unique!"));
			return;
		}
	}
	catch (ATN::Exception e) {}

	ATN::Manager::updateName(*m_network, name.toStdString());
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

void UI_NetworkContainer::findTransitions()
{
	emit findTransitionsRequest(m_network->id());
}

void UI_NetworkContainer::receiveOpenNetworkRequest(int id)
{
	emit openNetworkRequest(id);
}

void UI_NetworkContainer::receiveStateLayoutRequest()
{
	layoutStates();
}

void UI_NetworkContainer::createNewConnector()
{
	UI_ConnectorStart* start = (UI_ConnectorStart*)sender();

	UI_Connector *connector = new UI_Connector(ui.networkContents);

	connector->setNetwork(&m_proxy);

	start->setConnector(connector);
	connector->setStart(start);

	connector->show();
}

void UI_NetworkContainer::createNewTransition()
{
	UI_NetworkState* uiStateFrom = (UI_NetworkState*)sender();
	UI_NetworkState* uiStateTo = (UI_NetworkState*)uiStateFrom->ui.connectorOut->transitionConnector()->connector()->end()->parent();

	ATN::Transition *t = new ATN::Transition();

	t->setID(ATN::Manager::maxID() + 1);
	t->setName(std::string("Transition ") + std::to_string(uiStateFrom->m_state->transitions().size() + 1));
	t->setState(uiStateTo->m_state);

	// Percept must always be set
	t->setPercept(ATN::Manager::getPercepts()[0]);

	// Add this object to the same ATN list the network belongs to
	ATN::List<ATN::Entry> *outList;

	ATN::Manager::findByID(m_network->id(), outList);

	outList->add(*(ATN::Entry*)t);
	ATN::Manager::addEntry(*(ATN::Entry*)t);

	uiStateFrom->m_state->add(*t);

	UI_NetworkTransition *uiTransition = new UI_NetworkTransition(uiStateFrom);

	uiTransition->m_state = uiStateFrom;

	uiTransition->initialize(t, m_network);

	UI_Connector *uiConnector = new UI_Connector(ui.networkContents);

	uiConnector->setNetwork(&m_proxy);

	uiConnector->setStart(uiTransition->m_connector);
	uiConnector->setEnd(uiStateTo->ui.connectorIn);

	uiTransition->m_connector->setConnector(uiConnector);

	connect(uiTransition->m_connector, SIGNAL(createNewConnector()), this, SLOT(createNewConnector()));
	connect(uiTransition->m_connector, SIGNAL(establishTransition()), this, SLOT(updateTransition()));
	connect(uiTransition, SIGNAL(unlockTransitionEditor()), this, SLOT(editTransition()));

	uiTransition->show();
	uiConnector->show();

	uiStateFrom->ui.connectorOut->addTransition(uiTransition);

	int height = uiStateFrom->height();

	uiStateFrom->adjustSize();
	uiStateFrom->setFixedHeight(height);

	// The transition connector only made a temporary connection, now we delete it since we have recreated it as the transition
	uiStateFrom->ui.connectorOut->transitionConnector()->deleteConnector();

	layoutStates();

	// If the new state was added here, then the current edited state can now be moved downwards
	if (m_currentEditState != nullptr && m_currentEditState == uiStateFrom)
	{
		ui.buttonTransitionSortDown->setEnabled(true);
	}
}

void UI_NetworkContainer::updateTransition()
{
	UI_NetworkTransition *ut = (UI_NetworkTransition*)sender()->parent();

	UI_NetworkState *state = (UI_NetworkState*)ut->m_connector->connector()->end()->parent();

	ut->transition()->setState(state->m_state);
}

void UI_NetworkContainer::editTransition()
{
	if (m_currentEditTransition != nullptr)
	{
		m_currentEditTransition->setHighlighted(false);
	}

	UI_NetworkTransition *uiTransition = (UI_NetworkTransition*)sender();

	ATN::Transition *transition = uiTransition->transition();


	uiTransition->setHighlighted(true);

	// Prevent that we try to update the transition as we load in current values
	m_currentEditState = nullptr;
	m_currentEditTransition = nullptr;

	ui.textTransitionName->setText(QString::fromStdString(transition->name()));
	ui.textTransitionID->setText(QString::fromStdString(std::to_string(transition->id())));

	ui.labelTransitionInterpretation->setText(QString::fromStdString(std::string("Interpretation: ") + uiTransition->interpret()));

	const std::vector<ATN::Percept*> &percepts = ATN::Manager::getPercepts();

	for (size_t i = 0; i < percepts.size(); i++)
	{
		ATN::Percept *percept = percepts[i];

		// Comparing using ID since some effects/percepts are defined in multiple files
		// which doesn't matter since the game only cares about IDs and we don't allow effects/percepts to be modified
		if (transition->percept()->id() == percept->id())
		{
			ui.comboTransitionPercept->setCurrentIndex(i);
			break;
		}
	}

	if (transition->effect() == nullptr)
	{
		ui.comboTransitionEffect->setCurrentIndex(0);
	}
	else
	{
		const std::vector<ATN::Effect*> &effects = ATN::Manager::getEffects();

		for (size_t i = 0; i < effects.size(); i++)
		{
			ATN::Effect *effect = effects[i];

			if (transition->effect()->id() == effect->id())
			{
				ui.comboTransitionEffect->setCurrentIndex(i + 1);
				break;
			}
		}
	}

	m_currentEditState = (UI_NetworkState*)uiTransition->m_state;
	m_currentEditTransition = uiTransition;

	for (size_t i = 0; i < m_currentEditState->m_state->transitions().size(); i++)
	{
		if (m_currentEditState->m_state->transitions()[i] == transition)
		{
			ui.buttonTransitionSortUp->setEnabled(i != 0);
			ui.buttonTransitionSortDown->setEnabled(i != m_currentEditState->m_state->transitions().size()-1);
			break;
		}
	}

	populateTransitionArguments(m_currentTransitionPerceptArguments, m_currentTransitionPerceptResources, ui.listTransitionPerceptArguments, ui.listTransitionPerceptResources, (ATN::IResourceHolder*)transition->percept(), transition->perceptParameterMarshalls(), transition->perceptResourceMarshalls());
	populateTransitionArguments(m_currentTransitionEffectArguments, m_currentTransitionEffectResources, ui.listTransitionEffectArguments, ui.listTransitionEffectResources, (ATN::IResourceHolder*)transition->effect(), transition->effectParameterMarshalls(), transition->effectResourceMarshalls());

	ui.frameTransition->setEnabled(true);
}

void UI_NetworkContainer::moveTransitionUp()
{
	m_currentEditState->m_state->moveUp(*m_currentEditTransition->transition());

	m_currentEditState->ui.connectorOut->moveTransitionUp(m_currentEditTransition);

	for (size_t i = 0; i < m_currentEditState->m_state->transitions().size(); i++)
	{
		if (m_currentEditState->m_state->transitions()[i] == m_currentEditTransition->transition())
		{
			ui.buttonTransitionSortUp->setEnabled(i != 0);
			ui.buttonTransitionSortDown->setEnabled(i != m_currentEditState->m_state->transitions().size() - 1);
			break;
		}
	}
}

void UI_NetworkContainer::moveTransitionDown()
{
	m_currentEditState->m_state->moveDown(*m_currentEditTransition->transition());

	m_currentEditState->ui.connectorOut->moveTransitionDown(m_currentEditTransition);

	for (size_t i = 0; i < m_currentEditState->m_state->transitions().size(); i++)
	{
		if (m_currentEditState->m_state->transitions()[i] == m_currentEditTransition->transition())
		{
			ui.buttonTransitionSortUp->setEnabled(i != 0);
			ui.buttonTransitionSortDown->setEnabled(i != m_currentEditState->m_state->transitions().size() - 1);
			break;
		}
	}
}

void UI_NetworkContainer::deleteTransition()
{
	m_currentEditState->m_state->remove(*m_currentEditTransition->transition());

	m_currentEditState->ui.connectorOut->removeTransition(m_currentEditTransition);

	deleteATN(m_currentEditTransition->transition());

	m_currentEditTransition->deleteLater();

	ui.frameTransition->setEnabled(false);

	m_currentEditState = nullptr;
	m_currentEditTransition = nullptr;
}

void UI_NetworkContainer::repopulateArguments()
{
	for (UI_NetworkState *uiState : m_states)
	{
		uiState->populateArguments();
	}

	if (m_currentEditState != nullptr)
	{
		ui.labelTransitionInterpretation->setText(QString::fromStdString(std::string("Interpretation: ") + m_currentEditTransition->interpret()));

		ATN::Transition *transition = m_currentEditTransition->transition();

		populateTransitionArguments(m_currentTransitionPerceptArguments, m_currentTransitionPerceptResources, ui.listTransitionPerceptArguments, ui.listTransitionPerceptResources, (ATN::IResourceHolder*)transition->percept(), transition->perceptParameterMarshalls(), transition->perceptResourceMarshalls());
		populateTransitionArguments(m_currentTransitionEffectArguments, m_currentTransitionEffectResources, ui.listTransitionEffectArguments, ui.listTransitionEffectResources, (ATN::IResourceHolder*)transition->effect(), transition->effectParameterMarshalls(), transition->effectResourceMarshalls());
	}
}

void UI_NetworkContainer::maintainEditFramePositions()
{
	int curDistance = ui.scrollArea->horizontalScrollBar()->value();

	ui.frameNetwork->move(m_initialFrameNetworkPos + QPoint(curDistance, 0));
	ui.frameTransition->move(m_initialFrameTransitionPos + QPoint(curDistance, 0));

	// The thread connectors must be redrawn as the origin has moved
	for (UI_NetworkThread *ut : m_threads)
	{
		ut->ui.connector->updateConnector();
	}
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

		connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(resourceMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(resourceMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(resourceRemove()));

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

		ut->blockSignals(true);

		// Remove placeholder option
		ut->ui.variableType->clear();

		ut->ui.variableType->addItems(m_variableTypes);
		ut->ui.variableType->setCurrentIndex(ut->ui.variableType->findText(QString::fromStdString(p->m_type)));

		ut->ui.variableDesc->setText(QString::fromStdString(p->m_desc));

		ut->loadTranslations();

		QString strValue = QString::fromStdString(p->translateValue(p->m_defaultValue));

		int index = ut->ui.variableValue->findText(strValue);

		if (index != -1)
			ut->ui.variableValue->setCurrentIndex(index);
		else
			ut->ui.variableValue->setCurrentText(strValue);

		ut->blockSignals(false);

		connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(variableMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(variableMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(variableRemove()));
		connect(ut, SIGNAL(changeType(QString)), this, SLOT(variableTypeChange(QString)));
		connect(ut, SIGNAL(repopulateArguments()), this, SLOT(repopulateArguments()));

		m_variables.push_back(ut);
	}

	layoutSortables(m_variables, ui.listNetworkVariables);
}

void UI_NetworkContainer::layoutStates()
{
	int x = 0, y = 0;

	std::vector<QRect> stateSpaces;

	for (size_t i = 0; i < m_states.size(); i++)
	{
		UI_NetworkState *ut = m_states[i];

		// Disable first and last sort buttons
		ut->ui.buttonSortUp->setEnabled(i != 0);
		ut->ui.buttonSortDown->setEnabled(i != m_states.size() - 1);

		ut->move(x, 0);

		x += ut->width() + STATE_MARGIN;

		// Important that these positions are in the same coordinate space as UI_Connector
		QPoint pos = ut->pos();

		QRect rect(pos.x(), pos.y(), ut->width(), ut->height());

		stateSpaces.push_back(rect);
	}

	ui.frameStates->adjustSize();

	ui.frameStates->resize(ui.frameStates->width() + STATE_MARGIN, ui.frameStates->height());

	// Adjust scroller to recognize the total area
	ui.networkContents->setMinimumWidth(std::max(m_networkContentsMinimumSize.width(), ui.frameStates->width() + ui.frameStates->x()));
	ui.networkContents->setMinimumHeight(std::max(m_networkContentsMinimumSize.height(), ui.frameStates->height() + ui.frameStates->y() + NetworkContainerProxy::CONNECTOR_HEIGHT_OFFSET*4));

	m_proxy.setUpperHeight(ui.frameStates->y());
	m_proxy.setLowerHeight(ui.frameStates->y() + ui.frameStates->height());

	m_proxy.setStateSpaces(stateSpaces);

	ui.networkContents->update();
}

void UI_NetworkContainer::initializeStates()
{
	std::unordered_map<const ATN::State*, UI_NetworkState*> uiStates;

	for (ATN::State *s : m_network->states())
	{
		UI_NetworkState *ut = new UI_NetworkState(ui.frameStates);

		ut->initialize(s, m_network);

		connect(ut->ui.buttonSortUp, SIGNAL(clicked()), this, SLOT(stateMoveUp()));
		connect(ut->ui.buttonSortDown, SIGNAL(clicked()), this, SLOT(stateMoveDown()));
		connect(ut->ui.buttonDelete, SIGNAL(clicked()), this, SLOT(stateRemove()));

		connect(ut, SIGNAL(openNetworkRequest(int)), this, SLOT(receiveOpenNetworkRequest(int)));
		connect(ut, SIGNAL(requestLayout()), this, SLOT(receiveStateLayoutRequest()));
		connect(ut, SIGNAL(requestNewTransition()), this, SLOT(createNewTransition()));

		connect(ut->ui.connectorOut->transitionConnector(), SIGNAL(createNewConnector()), this, SLOT(createNewConnector()));

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

			uiTransition->m_state = uiStateFrom;

			uiTransition->initialize(t, m_network);

			UI_NetworkState *uiStateTo = uiStates[t->state()];

			UI_Connector *uiConnector = new UI_Connector(ui.networkContents);

			uiConnector->setNetwork(&m_proxy);

			uiConnector->setStart(uiTransition->m_connector);
			uiConnector->setEnd(uiStateTo->ui.connectorIn);

			uiTransition->m_connector->setConnector(uiConnector);

			connect(uiTransition->m_connector, SIGNAL(createNewConnector()), this, SLOT(createNewConnector()));
			connect(uiTransition->m_connector, SIGNAL(establishTransition()), this, SLOT(updateTransition()));
			connect(uiTransition, SIGNAL(unlockTransitionEditor()), this, SLOT(editTransition()));

			uiStateFrom->ui.connectorOut->addTransition(uiTransition);

			int height = uiStateFrom->height();

			uiStateFrom->adjustSize();
			uiStateFrom->setFixedHeight(height);
		}
	}

	layoutStates();
}

void UI_NetworkContainer::populateTransitionArguments(std::vector<UI_InputArgument*> &argumentList, std::vector<UI_InputResource*> &resourceList, QWidget *argumentWidget, QWidget *resourceWidget, const ATN::IResourceHolder *resourceHolder, const std::vector<ATN::ParameterMarshall*> paramMarshalls, const std::vector<ATN::ResourceMarshall*> resourceMarshalls)
{
	for (UI_InputArgument *ut : argumentList)
		ut->deleteLater();
	for (UI_InputResource *ut : resourceList)
		ut->deleteLater();

	argumentList.clear();
	resourceList.clear();

	int x = 0, y = 0;

	for (size_t i = 0; i < resourceMarshalls.size(); i++)
	{
		UI_InputResource *ut = new UI_InputResource(resourceWidget);

		ut->initialize(resourceMarshalls[i], resourceHolder->resources()[i], m_network);

		ut->move(0, y);
		ut->show();

		y += ut->size().height();

		ut->show();

		resourceList.push_back(ut);
	}

	resourceWidget->setMinimumHeight(y);

	x = 0, y = 0;

	for (size_t i = 0; i < paramMarshalls.size(); i++)
	{
		UI_InputArgument *ut = new UI_InputArgument(argumentWidget);

		ut->initialize(paramMarshalls[i], resourceHolder->parameters()[i], m_network);

		ut->move(0, y);
		ut->show();

		y += ut->size().height();

		ut->show();

		argumentList.push_back(ut);
	}

	argumentWidget->setMinimumHeight(y);
}

UI_NetworkContainer::UI_NetworkContainer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_networkContentsMinimumSize = ui.networkContents->minimumSize();

	// Only enabled when highlighting a transition
	ui.frameTransition->setEnabled(false);

	initializeArgumentLists();

	m_initialFrameNetworkPos = ui.frameNetwork->pos();
	m_initialFrameTransitionPos = ui.frameTransition->pos();

	connect(ui.scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(maintainEditFramePositions()));
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

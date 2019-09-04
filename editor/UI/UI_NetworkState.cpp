#include "UI_NetworkState.h"

void UI_NetworkState::populateArguments()
{
	for (UI_InputArgument *ut : m_arguments)
		ut->deleteLater();
	for (UI_InputResource *ut : m_resources)
		ut->deleteLater();

	m_arguments.clear();
	m_resources.clear();

	int x = 0, y = 0;

	std::vector<const ATN::Resource*> resources;

	if (m_state->networkTransition() != nullptr)
	{
		for (const ATN::Resource *r : m_state->networkTransition()->resources())
		{
			if (!r->m_internalResource)
				resources.push_back(r);
		}
	}

	for (size_t i = 0; i < m_state->resourceMarshalls().size(); i++)
	{
		UI_InputResource *ut = new UI_InputResource(ui.listTransitionResources);

		ut->initialize(m_state->resourceMarshalls()[i], resources[i], m_network);

		ut->move(0, y);
		ut->show();

		y += ut->size().height();

		m_resources.push_back(ut);
	}

	ui.listTransitionResources->setMinimumHeight(y);

	x = 0, y = 0;

	for (size_t i = 0; i < m_state->parameterMarshalls().size(); i++)
	{
		UI_InputArgument *ut = new UI_InputArgument(ui.listTransitionArguments);

		ut->initialize(m_state->parameterMarshalls()[i], m_state->networkTransition()->parameters()[i], m_network);

		ut->move(0, y);
		ut->show();

		y += ut->size().height();

		m_arguments.push_back(ut);
	}

	ui.listTransitionArguments->setMinimumHeight(y);
}

UI_NetworkState::UI_NetworkState(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.connectorOut->transitionConnector(), SIGNAL(establishTransition()), this, SLOT(establishTransition()));

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(openContextMenu(const QPoint&)));
}

UI_NetworkState::~UI_NetworkState()
{

}

void UI_NetworkState::initialize(ATN::State *s, const ATN::Network *net)
{
	ui.textStateName->setText(QString::fromStdString(s->name()));
	ui.textUniqueID->setText(QString::fromStdString(std::to_string(s->id())));

	if (s->networkTransition() != nullptr)
	{
		ui.checkBoxExternalNetworkConnector->setChecked(true);
		enableExternalNetwork(true);

		ui.comboBoxExternalNetwork->setCurrentIndex(ui.comboBoxExternalNetwork->findText(QString::fromStdString(std::to_string(s->networkTransition()->id()) + std::string(": ") + s->networkTransition()->name())));
	}
	else
	{
		setMinimized(true);
	}

	m_state = s;

	m_network = net;

	populateArguments();

	ui.connectorOut->layout();
	adjustSize();
}

void UI_NetworkState::setMinimized(bool minimize)
{
	QWidget *widgets[] = { 
		ui.frame, ui.textStateName, ui.buttonSortUp, ui.buttonSortDown,
		ui.buttonDelete, ui.labelResources, ui.labelArguments,
		ui.scrollAreaResources, ui.scrollAreaArguments, ui.comboBoxExternalNetwork,
		ui.connectorOut
	};

	if (minimize)
	{
		if (m_geometries.size() == 0)
		{
			for (QWidget *widget : widgets)
			{
				m_geometries.push_back(QRect(widget->geometry()));
			}
		}

		// Note: setGeometry is unreliable, so always use move and setFixedSize
		ui.textStateName->setFixedWidth(101);

		ui.buttonSortUp->move(200, 10);
		ui.buttonSortDown->move(220, 10);
		ui.buttonDelete->move(240, 10);

		ui.labelResources->setFixedWidth(121);
		ui.labelArguments->move(140, 30);
		ui.labelArguments->setFixedSize(121, 31);

		ui.scrollAreaResources->setFixedWidth(121);
		ui.scrollAreaArguments->move(140, 60);
		ui.scrollAreaArguments->setFixedSize(121, 121);

		ui.comboBoxExternalNetwork->setFixedWidth(111);

		ui.frame->setFixedWidth(271);

		ui.connectorOut->move(290, ui.connectorOut->y());
	}
	else
	{
		for (size_t i = 0; i < m_geometries.size(); i++)
		{
			widgets[i]->move(m_geometries[i].topLeft());
			widgets[i]->setFixedSize(m_geometries[i].size());
		}
	}

	ui.connectorOut->layout();

	int height = this->height();

	adjustSize();
	setFixedHeight(height);

	emit requestLayout();
}

void UI_NetworkState::setReadOnly(bool readonly)
{
	for (UI_InputArgument *ut : m_arguments)
		ut->ui.comboBox->setReadOnly(readonly);
	for (UI_InputResource *ut : m_resources)
		ut->ui.comboBox->setDisabled(readonly);
}

void UI_NetworkState::handleHighlight()
{
	emit requestHighlight();
}

void UI_NetworkState::handleCopy()
{
	ATN::Manager::setStoredEntry(m_state);
}

void UI_NetworkState::handlePaste()
{
	emit requestPaste();
}

void UI_NetworkState::handlePasteLimited()
{
	emit requestPasteLimited();
}

void UI_NetworkState::openContextMenu(const QPoint &pos)
{
	// Only react within frame
	if (pos.x() < ui.frame->x() || pos.x() > (ui.frame->x() + ui.frame->width()) || pos.y() < ui.frame->y() || pos.y() > (ui.frame->y() + ui.frame->height()))
		return;

	QMenu contextMenu(tr("Context menu"), this);

	QAction actionHighlight("Highlight incoming transitions", this);

	QAction actionCopy("Copy values and transitions", this);

	QAction actionPaste("Paste values and transitions", this);

	QAction actionPasteLimited("Paste values only", this);

	const ATN::Entry *curPaste = ATN::Manager::getStoredEntry();

	// Don't allow paste if there is none, or if it's the wrong type, or if we're disabled
	if (curPaste == nullptr || typeid(*curPaste) != typeid(ATN::State) || ui.textStateName->isReadOnly())
	{
		actionPaste.setEnabled(false);
		actionPasteLimited.setEnabled(false);
	}

	connect(&actionHighlight, SIGNAL(triggered()), this, SLOT(handleHighlight()));
	connect(&actionCopy, SIGNAL(triggered()), this, SLOT(handleCopy()));
	connect(&actionPaste, SIGNAL(triggered()), this, SLOT(handlePaste()));
	connect(&actionPasteLimited, SIGNAL(triggered()), this, SLOT(handlePasteLimited()));

	connect(&contextMenu, SIGNAL(destroyed()), this, SLOT(update()));

	contextMenu.addAction(&actionHighlight);
	contextMenu.addAction(&actionCopy);
	contextMenu.addAction(&actionPaste);
	contextMenu.addAction(&actionPasteLimited);

	contextMenu.exec(mapToGlobal(pos));
}

void UI_NetworkState::selectExternalNetwork(int index)
{
	if (m_state != nullptr && index != -1)
	{
		const std::string &str = ui.comboBoxExternalNetwork->itemText(index).toStdString();

		int netID = std::stoi(str.substr(0, str.find(':')));

		ATN::Network *net = (ATN::Network*)&ATN::Manager::findByID(netID);

		m_state->setNetworkTransition(net);

		populateArguments();
	}
}

void UI_NetworkState::setStateName(const QString &name)
{
	if (m_state != nullptr)
	{
		ATN::Manager::updateName(*m_state, name.toStdString());
	}
}

void UI_NetworkState::viewExternalNetwork()
{
	emit openNetworkRequest(m_state->networkTransition()->id());
}

void UI_NetworkState::establishTransition()
{
	emit requestNewTransition();
}

void UI_NetworkState::enableExternalNetwork(bool enable)
{
	ui.comboBoxExternalNetwork->setEnabled(enable);
	ui.buttonViewExternalNetwork->setEnabled(enable);
	setMinimized(!enable);

	if (enable)
	{
		ui.comboBoxExternalNetwork->clear();

		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			ui.comboBoxExternalNetwork->addItem(QString::fromStdString(std::to_string(net->id()) + std::string(": ") + net->name()));
		}
	}
	else
	{
		m_state->setNetworkTransition(nullptr);
		populateArguments();
	}
}

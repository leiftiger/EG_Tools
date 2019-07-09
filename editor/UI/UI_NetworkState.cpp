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
}

UI_NetworkState::~UI_NetworkState()
{

}

void UI_NetworkState::initialize(ATN::State *s, const ATN::Network *net)
{
	ui.textStateName->setText(QString::fromStdString(s->name()));
	ui.textUniqueID->setText(QString::fromStdString(std::to_string(s->id())));

	if (s->networkTransition() != nullptr && s->networkTransition() != net)
	{
		ui.checkBoxExternalNetworkConnector->setChecked(true);
		enableExternalNetwork(true);

		ui.comboBoxExternalNetwork->setCurrentIndex(ui.comboBoxExternalNetwork->findText(QString::fromStdString(std::to_string(s->networkTransition()->id()) + std::string(": ") + s->networkTransition()->name())));
	}

	m_state = s;

	m_network = net;

	populateArguments();
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
		m_state->setName(name.toStdString());
	}
}

void UI_NetworkState::viewExternalNetwork()
{
	emit openNetworkRequest(m_state->networkTransition()->id());
}

void UI_NetworkState::enableExternalNetwork(bool enable)
{
	ui.comboBoxExternalNetwork->setEnabled(enable);
	ui.buttonViewExternalNetwork->setEnabled(enable);

	if (enable)
	{
		ui.comboBoxExternalNetwork->clear();

		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			ui.comboBoxExternalNetwork->addItem(QString::fromStdString(std::to_string(net->id()) + std::string(": ") + net->name()));
		}
	}
}

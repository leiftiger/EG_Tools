#include "UI_NetworkState.h"

void UI_NetworkState::populateArguments(const ATN::Network *net)
{
	for (UI_InputArgument *ut : m_arguments)
		ut->deleteLater();
	for (UI_InputResource *ut : m_resources)
		ut->deleteLater();

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

		ut->initialize(m_state->resourceMarshalls()[i], resources[i], net);

		ut->move(0, y);

		y += ut->size().height();
	}

	x = 0, y = 0;

	for (size_t i = 0; i < m_state->parameterMarshalls().size(); i++)
	{
		UI_InputArgument *ut = new UI_InputArgument(ui.listTransitionArguments);

		ut->initialize(m_state->parameterMarshalls()[i], m_state->networkTransition()->parameters()[i], net);

		ut->move(0, y);

		y += ut->size().height();
	}

	ui.listTransitionArguments->setMinimumHeight(y);
}

UI_NetworkState::UI_NetworkState(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.labelExternalNetworkName->setText(tr(""));

	ui.connectOutNetwork->setConnectFlags(ConnectFlags::Above);
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
		ui.labelExternalNetworkName->setText(QString::fromStdString(std::string("<extern transition: ") + s->networkTransition()->name() + std::string(">")));
	}

	m_state = s;
	populateArguments(net);
}

void UI_NetworkState::selectExternalNetwork(int index)
{
	if (m_state != nullptr && index != -1)
	{
		const std::string &str = ui.comboBoxExternalNetwork->itemText(index).toStdString();

		int index = str.find(':');

		int netID = std::stoi(str.substr(0, index));

		ATN::Network *net = (ATN::Network*)&ATN::Manager::findByID(netID);

		m_state->setNetworkTransition(net);

		ui.labelExternalNetworkName->setText(QString::fromStdString(std::string("<extern transition: ") + net->name() + std::string(">")));
	}
}

void UI_NetworkState::setStateName(const QString &name)
{
	if (m_state != nullptr)
	{
		m_state->setName(name.toStdString());
	}
}

void UI_NetworkState::enableExternalNetwork(bool enable)
{
	ui.comboBoxExternalNetwork->setEnabled(enable);
	ui.connectOutNetwork->setHidden(enable);

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
		ui.labelExternalNetworkName->setText(tr(""));
	}
}

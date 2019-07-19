#include "UI_NetworkVariable.h"

UI_NetworkVariable::UI_NetworkVariable(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_NetworkVariable::~UI_NetworkVariable()
{
}

void UI_NetworkVariable::loadTranslations()
{
	m_populating = true;

	// Always clear from previous type so the user is required to input a new one
	ui.variableValue->clear();
	ui.variableValue->setCurrentText(QString::fromStdString(m_variable->translateValue(0)));

	std::string variableType = m_variable->m_type;

	if (ATN::Manager::hasDefinitions(variableType))
	{
		QStringList translations;

		ATN::List<ATN::Property> &defs = ATN::Manager::getDefinitions(variableType);

		for (const std::pair<std::uint32_t, ATN::IATN_Data*> &pair : defs)
		{
			translations.append(QString::fromStdString(pair.second->name()));
		}

		translations.sort(Qt::CaseSensitivity::CaseSensitive);

		ui.variableValue->addItems(translations);
	}
	// Networks are handled separately due to being pointers
	else if (variableType == "Network ID")
	{
		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			ui.variableValue->addItem(QString::fromStdString(std::to_string(net->id()) + std::string(": ") + net->name()));
		}
	}

	m_populating = false;
}

void UI_NetworkVariable::setVariableType(const QString &type)
{
	if (m_variable == nullptr)
		return;

	emit changeType(type);
}

void UI_NetworkVariable::setVariableValue(const QString &value)
{
	if (m_variable == nullptr || m_populating || signalsBlocked())
		return;

	ui.variableValue->setInputError(false);

	if (value.size() == 0)
	{
		ui.variableValue->setInputError(true);
		return;
	}

	try
	{
		m_variable->m_defaultValue = m_variable->translateName(value.toStdString());
	}
	catch (std::exception e)
	{
		ui.variableValue->setInputError(true);
		QToolTip::showText(ui.variableValue->mapToGlobal(ui.variableValue->pos()), tr("Invalid input"));
	}
}

void UI_NetworkVariable::setVariableName(const QString &name)
{
	m_variable->m_desc = name.toStdString();

	emit repopulateArguments();
}

#include "UI_NetworkParameter.h"

UI_NetworkParameter::UI_NetworkParameter(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_NetworkParameter::~UI_NetworkParameter()
{
}

void UI_NetworkParameter::loadTranslations()
{
	m_populating = true;

	// Always clear from previous type so the user is required to input a new one
	ui.paramValue->clear();
	ui.paramValue->setCurrentText(QString::fromStdString(m_parameter->translateValue(0)));

	std::string parameterType = m_parameter->m_type;

	if (ATN::Manager::hasDefinitions(parameterType))
	{
		QStringList translations;

		ATN::List<ATN::Property> &defs = ATN::Manager::getDefinitions(parameterType);

		for (const std::pair<std::uint32_t, ATN::IATN_Data*> &pair : defs)
		{
			translations.append(QString::fromStdString(pair.second->name()));
		}

		QCollator collator;
		collator.setNumericMode(true);
		collator.setCaseSensitivity(Qt::CaseSensitivity::CaseSensitive);

		qSort(translations.begin(), translations.end(), collator);

		ui.paramValue->addItems(translations);
	}
	// Networks are handled separately due to being pointers
	else if (parameterType == "Network ID")
	{
		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			if (net->parameters().size() == 0 && !net->hasResourceInputs())
				ui.paramValue->addItem(QString::fromStdString(std::to_string(net->id()) + std::string(": ") + net->name()));
		}
	}

	m_populating = false;
}

void UI_NetworkParameter::setParameterType(const QString &type)
{
	if (m_parameter == nullptr)
		return;

	emit changeType(type);
}

void UI_NetworkParameter::setParameterValue(const QString &value)
{
	if (m_parameter == nullptr || m_populating || signalsBlocked())
		return;

	ui.paramValue->setInputError(false);

	if (value.size() == 0)
	{
		ui.paramValue->setInputError(true);
		return;
	}

	try
	{
		m_parameter->m_defaultValue = m_parameter->translateName(value.toStdString());
	}
	catch (std::exception e)
	{
		ui.paramValue->setInputError(true);
		QToolTip::showText(ui.paramValue->mapToGlobal(ui.paramValue->pos()), tr("Invalid input"));
	}
}

void UI_NetworkParameter::setParameterName(const QString &name)
{
	m_parameter->m_desc = name.toStdString();

	emit repopulateArguments();
}

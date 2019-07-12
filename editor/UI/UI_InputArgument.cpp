#include "UI_InputArgument.h"

UI_InputArgument::UI_InputArgument(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_InputArgument::~UI_InputArgument()
{
}

void UI_InputArgument::initialize(ATN::ParameterMarshall *argument, const ATN::Parameter *parameter, const ATN::Network *net)
{
	// Fetch parameter name
	ui.label->setText(QString::fromStdString(parameter->m_desc + std::string(":")));
	ui.label->setToolTip(ui.label->text());

	ui.comboBox->setToolTip(QString::fromStdString(parameter->m_type));

	ui.comboBox->clear();

	for (size_t i = 0; i < net->parameters().size(); i++)
	{
		ATN::Parameter *netParam = net->parameters()[i];

		if (netParam->m_type == parameter->m_type)
		{
			char str[STR_FORMAT_BUFF];

			sprintf_s(str, "[%d: %s]", (int)i, netParam->m_desc.c_str());

			ui.comboBox->addItem(QString::fromStdString(str));

			if (argument->m_type == ATN::ParameterMarshallType::ParameterIndex && argument->m_value == i)
			{
				ui.comboBox->setCurrentIndex(ui.comboBox->count() - 1);
			}
		}
	}

	if (ATN::Manager::hasDefinitions(parameter->m_type))
	{
		QStringList translations;

		ATN::List<ATN::Property> &defs = ATN::Manager::getDefinitions(parameter->m_type);

		for (const std::pair<std::uint32_t, ATN::IATN_Data*> &pair : defs)
		{
			translations.append(QString::fromStdString(pair.second->name()));
		}

		translations.sort(Qt::CaseSensitivity::CaseSensitive);

		ui.comboBox->addItems(translations);
	}

	// Since networks are special by being pointers, we handle them manually
	if (parameter->m_type == "Network ID")
	{
		for (ATN::Network *net : ATN::Manager::getNetworks())
		{
			ui.comboBox->addItem(QString::fromStdString(std::to_string(net->id()) + std::string(": ") + net->name()));
		}
	}

	if (argument->m_type != ATN::ParameterMarshallType::ParameterIndex)
	{
		QString strValue = QString::fromStdString(parameter->translateValue(argument->m_value));

		int index = ui.comboBox->findText(strValue);

		if (index != -1)
			ui.comboBox->setCurrentIndex(index);
		else
			ui.comboBox->setCurrentText(strValue);
	}

	m_parameter = argument;
}

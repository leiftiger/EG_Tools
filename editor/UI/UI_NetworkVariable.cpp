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
	// Always clear from previous type so the user is required to input a new one
	ui.variableValue->clear();

	std::string variableType = ui.variableType->currentText().toStdString();

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
}

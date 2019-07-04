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

	if (ATN::Manager::hasDefinitions(ui.variableType->currentText().toStdString()))
	{
		QStringList translations;

		ATN::List<ATN::Property> defs = ATN::Manager::getDefinitions(ui.variableType->currentText().toStdString());

		for (const std::pair<std::uint32_t, ATN::IATN_Data*> &pair : defs)
		{
			translations.append(QString::fromStdString(pair.second->name()));
		}

		translations.sort(Qt::CaseSensitivity::CaseSensitive);

		ui.variableValue->addItems(translations);
	}
}

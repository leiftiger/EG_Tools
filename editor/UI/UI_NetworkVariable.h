#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkVariable.h"

#include "ATN/ATN_Manager.h"
#include "ATN/ATN_Resources.h"

class UI_NetworkVariable : public QWidget
{
	Q_OBJECT

public:
	UI_NetworkVariable(QWidget *parent = Q_NULLPTR);
	~UI_NetworkVariable();

	Ui::UI_NetworkVariable ui;

	ATN::Parameter *m_variable;

	// Populates the default value list with known translations
	void loadTranslations();
};

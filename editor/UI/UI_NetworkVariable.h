#pragma once

#include <QWidget>
#include <QToolTip>
#include "GeneratedFiles/ui_UI_NetworkVariable.h"

#include "ATN/ATN_Manager.h"
#include "ATN/ATN_Resources.h"

class UI_NetworkVariable : public QWidget
{
	Q_OBJECT

private:
	bool m_populating = false;

public:
	UI_NetworkVariable(QWidget *parent = Q_NULLPTR);
	~UI_NetworkVariable();

	Ui::UI_NetworkVariable ui;

	ATN::Parameter *m_variable;

	// Populates the default value list with known translations
	void loadTranslations();

public slots:

	void setVariableType(const QString &type);
	void setVariableValue(const QString &value);
	void setVariableName(const QString &name);

signals:
	void repopulateArguments();
	void changeType(const QString &type);
};

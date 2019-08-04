#pragma once

#include <QWidget>
#include <QToolTip>
#include <QCollator>
#include "GeneratedFiles/ui_UI_NetworkParameter.h"

#include "ATN/ATN_Manager.h"
#include "ATN/ATN_Resources.h"

class UI_NetworkParameter : public QWidget
{
	Q_OBJECT

private:
	bool m_populating = false;

public:
	UI_NetworkParameter(QWidget *parent = Q_NULLPTR);
	~UI_NetworkParameter();

	Ui::UI_NetworkParameter ui;

	ATN::Parameter *m_parameter;

	// Populates the default value list with known translations
	void loadTranslations();

public slots:

	void setParameterType(const QString &type);
	void setParameterValue(const QString &value);
	void setParameterName(const QString &name);

signals:
	void repopulateArguments();
	void changeType(const QString &type);
};

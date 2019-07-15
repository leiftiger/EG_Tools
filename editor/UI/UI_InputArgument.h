#pragma once

#include <QWidget>
#include <QToolTip>
#include "GeneratedFiles/ui_UI_InputArgument.h"

#include "ATN/ATN_Network.h"
#include "ATN/ATN_Resources.h"

class UI_InputArgument : public QWidget
{
	Q_OBJECT

private:

	const ATN::Parameter *m_parameter = nullptr;

	ATN::ParameterMarshall *m_argument = nullptr;

	std::vector<int> m_variables;

	static const int STR_FORMAT_BUFF = 1024;

public:
	UI_InputArgument(QWidget *parent = Q_NULLPTR);
	~UI_InputArgument();

	Ui::UI_InputArgument ui;

	// Initializes this input with available argument options and select the current argument
	void initialize(ATN::ParameterMarshall *argument, const ATN::Parameter *parameter, const ATN::Network *net);

public slots:
	void setArgument(const QString &str);

signals:
	void updated();
};

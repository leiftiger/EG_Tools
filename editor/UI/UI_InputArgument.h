#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_InputArgument.h"

#include "ATN/ATN_Network.h"
#include "ATN/ATN_Resources.h"

class UI_InputArgument : public QWidget
{
	Q_OBJECT

private:

	ATN::ParameterMarshall *m_parameter = nullptr;

	static const int STR_FORMAT_BUFF = 1024;

public:
	UI_InputArgument(QWidget *parent = Q_NULLPTR);
	~UI_InputArgument();

	Ui::UI_InputArgument ui;

	// Initializes this input with available argument options and select the current argument
	void initialize(ATN::ParameterMarshall *argument, const ATN::Parameter *parameter, const ATN::Network *net);
};

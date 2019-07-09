#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_NetworkTransition.h"

#include "UI_ConnectorStart.h"

#include "ATN/ATN_Transition.h"

class UI_NetworkTransition : public QWidget
{
	Q_OBJECT

private:

	static const int LABEL_PADDING = 5;

public:
	UI_NetworkTransition(QWidget *parent = Q_NULLPTR);
	~UI_NetworkTransition();

	Ui::UI_NetworkTransition ui;

	UI_ConnectorStart *m_connector;

	ATN::Transition *m_transition;

	// Returns an user-friendly string of what this transition does
	std::string interpret();

	void layout();
};

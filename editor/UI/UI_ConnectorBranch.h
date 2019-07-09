#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorBranch.h"

#include <vector>

#include "UI_NetworkTransition.h"
#include "UI_ConnectorStart.h"

// Forward declared
class UI_ConnectorStart;

class UI_ConnectorBranch : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorBranch(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorBranch();

	virtual void paintEvent(QPaintEvent *e) override;

	void addTransition(UI_NetworkTransition *ut);

private:
	Ui::UI_ConnectorBranch ui;

	std::vector<UI_NetworkTransition*> m_transitions;

	UI_ConnectorStart *m_newTransitionConnector;

	// Padding between transitions
	static const int TRANSITION_PADDING_BOTTOM = 20;

	// Padding between transitions
	static const int TRANSITION_PADDING_LEFTRIGHT = 5;

	void layout();
};

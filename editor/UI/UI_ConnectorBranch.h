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

private:
	Ui::UI_ConnectorBranch ui;

	std::vector<UI_NetworkTransition*> m_transitions;
	std::vector<UI_ConnectorStart*> m_transitionConnectors;

	UI_ConnectorStart *m_newTransitionConnector;

	const double CONNECTOR_SIZE = 4;
};

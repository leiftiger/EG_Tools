#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorBranch.h"

#include <vector>

#include "UI_NetworkTransition.h"
#include "UI_ConnectorStart.h"

// Forward declared
class UI_ConnectorStart; class UI_NetworkTransition;

class UI_ConnectorBranch : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorBranch(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorBranch();

	virtual void paintEvent(QPaintEvent *e) override;

	void addTransition(UI_NetworkTransition *ut);
	void moveTransitionUp(UI_NetworkTransition *ut);
	void moveTransitionDown(UI_NetworkTransition *ut);
	std::vector<UI_NetworkTransition*>::iterator removeTransition(UI_NetworkTransition *ut);

	const std::vector<UI_NetworkTransition*> transitions() const;

	void layout();

	UI_ConnectorStart *transitionConnector();

private:
	Ui::UI_ConnectorBranch ui;

	std::vector<UI_NetworkTransition*> m_transitions;

	UI_ConnectorStart *m_newTransitionConnector;

	int m_centerHeight = -1;

	// Padding between transitions
	static const int TRANSITION_PADDING_BOTTOM = 20;

	// Maximum number of transitions capable of being displayed with above padding
	// once breached, we use the tight padding
	static const int TRANSITION_PADDING_BOTTOM_THRESHOLD = 6;

	// If we have more than the above transitions, we should use a smaller padding to fit all of them
	static const int TRANSITION_PADDING_BOTTOM_TIGHT = 2;

	// Padding between transitions
	static const int TRANSITION_PADDING_LEFTRIGHT = 5;
};

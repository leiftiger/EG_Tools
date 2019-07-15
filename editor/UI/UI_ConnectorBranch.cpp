#include "UI_ConnectorBranch.h"

UI_ConnectorBranch::UI_ConnectorBranch(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_newTransitionConnector = new UI_ConnectorStart(parent);
	m_newTransitionConnector->setFixedSize(21, 21);

	layout();
}

UI_ConnectorBranch::~UI_ConnectorBranch()
{
}

void UI_ConnectorBranch::paintEvent(QPaintEvent *e)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Connector bounds

	painter.setPen(QPen(QColor(0, 0, 0, 255), CONNECTOR_SIZE));
	painter.setBrush(Qt::BrushStyle::NoBrush);

	int totalBranches = m_transitions.size() + 1;

	QPointF center = QPointF(width()*0.5f, height()*0.5f);

	painter.drawLine(0, center.y() - CONNECTOR_SIZE/2, center.x(), center.y() - CONNECTOR_SIZE / 2);

	if (totalBranches > 1)
	{
		painter.drawLine(center.x(), 0, center.x(), height());

		double branchHeight = (height() - CONNECTOR_SIZE) / (totalBranches - 1);

		double branchY = CONNECTOR_SIZE / 2;

		for (int i = 0; i < totalBranches; i++)
		{
			// Ensure the middle branch is centered properly and not off by one when we have odd branches
			if (i == totalBranches / 2)
				branchY -= 1;

			painter.drawLine(center.x(), branchY, width(), branchY);

			branchY += branchHeight;

			if (i == totalBranches - 2)
				branchY = (height() - CONNECTOR_SIZE/2);
		}
	}
	else
	{
		//painter.drawLine(center.x(), center.y()-1, width(), center.y()-1);
	}

	painter.end();
}

void UI_ConnectorBranch::addTransition(UI_NetworkTransition *ut)
{
	ut->layout();

	m_transitions.push_back(ut);

	layout();
	update();
}

void UI_ConnectorBranch::moveTransitionUp(UI_NetworkTransition *ut)
{
	m_transitions.insert(--removeTransition(ut), ut);

	layout();
}

void UI_ConnectorBranch::moveTransitionDown(UI_NetworkTransition *ut)
{
	m_transitions.insert(++removeTransition(ut), ut);

	layout();
}

std::vector<UI_NetworkTransition*>::iterator UI_ConnectorBranch::removeTransition(UI_NetworkTransition *ut)
{
	for (std::vector<UI_NetworkTransition*>::iterator it = m_transitions.begin(); it != m_transitions.end(); it++)
	{
		if (*it == ut)
		{
			std::vector<UI_NetworkTransition*>::iterator itNext = m_transitions.erase(it);

			layout();

			return itNext;
		}
	}

	layout();

	return m_transitions.end();
}

const std::vector<UI_NetworkTransition*> UI_ConnectorBranch::transitions() const
{
	return m_transitions;
}

void UI_ConnectorBranch::layout()
{
	int totalBranches = m_transitions.size() + 1;

	int transitionPadding = TRANSITION_PADDING_BOTTOM;

	if (totalBranches > TRANSITION_PADDING_BOTTOM_THRESHOLD)
	{
		transitionPadding = TRANSITION_PADDING_BOTTOM_TIGHT;
	}

	int desiredHeight = m_newTransitionConnector->height()/2;

	int farthestConnectorDistance = 0;

	for (UI_NetworkTransition *ut : m_transitions)
	{
		desiredHeight += ut->height() + transitionPadding;

		if (ut->m_connector->mapToParent(ut->m_connector->center()).x() > farthestConnectorDistance)
			farthestConnectorDistance = ut->m_connector->mapToParent(ut->m_connector->center()).x();
	}

	// Remove the space unused by the first element and add missing odd halves from connector and first element
	if (totalBranches > 1)
	{
		desiredHeight -= m_transitions[0]->height() / 2;

		desiredHeight += 2;
	}

	// Keep track of what height the designer set for us, to ensure we're always in center
	if (m_centerHeight == -1)
		m_centerHeight = height() / 2;

	setFixedHeight(desiredHeight);

	// Align the branch to center on same plane as connector
	desiredHeight -= CONNECTOR_SIZE + 1;

	move(x(), m_centerHeight - desiredHeight / 2);

	int branchY = 0;

	for (UI_NetworkTransition *ut : m_transitions)
	{
		QPoint pos = mapToGlobal(QPoint(width() + TRANSITION_PADDING_LEFTRIGHT, branchY - (ut->height() / 2)));

		ut->move(parentWidget()->mapFromGlobal(pos));

		branchY += ut->height() + transitionPadding;

		// Set a longer offset for transitions that are further in
		int dist = farthestConnectorDistance - ut->m_connector->mapToParent(ut->m_connector->center()).x();

		ut->m_connector->setConnectorOffset(dist + CONNECTOR_OFFSET);
		ut->m_connector->update();
	}

	int connectorX = width();

	// With only one connection, we only show the initial branching point with the connector closer to it
	if (totalBranches == 1)
	{
		connectorX = width() / 2;
		branchY += m_newTransitionConnector->height() / 4 - 1;
	}

	m_newTransitionConnector->move(parentWidget()->mapFromGlobal(mapToGlobal(QPoint(connectorX + TRANSITION_PADDING_LEFTRIGHT, branchY - (m_newTransitionConnector->height() / 2)))));

	if (totalBranches > 1)
	{
		int dist = farthestConnectorDistance - m_newTransitionConnector->width()/2;

		m_newTransitionConnector->setConnectorOffset(dist + CONNECTOR_OFFSET);
	}
	else
	{
		m_newTransitionConnector->setConnectorOffset(CONNECTOR_OFFSET);
	}
}

UI_ConnectorStart *UI_ConnectorBranch::transitionConnector()
{
	return m_newTransitionConnector;
}
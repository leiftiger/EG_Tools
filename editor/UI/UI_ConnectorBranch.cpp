#include "UI_ConnectorBranch.h"

UI_ConnectorBranch::UI_ConnectorBranch(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_newTransitionConnector = new UI_ConnectorStart(parent);
	m_newTransitionConnector->setFixedSize(21, 21);
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
			painter.drawLine(center.x(), branchY, width(), branchY);

			branchY += branchHeight;

			if (i == totalBranches - 2)
				branchY = (height() - CONNECTOR_SIZE/2);
		}
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

void UI_ConnectorBranch::layout()
{
	int totalBranches = m_transitions.size() + 1;

	int transitionPadding = TRANSITION_PADDING_BOTTOM;

	if (totalBranches > TRANSITION_PADDING_BOTTOM_THRESHOLD)
	{
		transitionPadding = TRANSITION_PADDING_BOTTOM_TIGHT;
	}

	int desiredHeight = m_newTransitionConnector->height() - transitionPadding;

	for (UI_NetworkTransition *ut : m_transitions)
	{
		desiredHeight += ut->height() + transitionPadding;
	}

	QPointF center = QPointF(width()*0.5f, height()*0.5f);

	setFixedHeight(desiredHeight);

	move(x(), y() + center.y() - desiredHeight / 2);

	int branchY = 0;

	for (UI_NetworkTransition *ut : m_transitions)
	{
		QPoint pos = mapToGlobal(QPoint(width() + TRANSITION_PADDING_LEFTRIGHT, branchY - (ut->height() / 2)));

		ut->move(parentWidget()->mapFromGlobal(pos));

		branchY += ut->height() + transitionPadding;
	}

	m_newTransitionConnector->move(parentWidget()->mapFromGlobal(mapToGlobal(QPoint(width() + TRANSITION_PADDING_LEFTRIGHT, branchY - (m_newTransitionConnector->height() / 2)))));
}

#include "UI_Connector.h"

UI_Connector::UI_Connector(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_Connector::~UI_Connector()
{
}

bool UI_Connector::hovered() const
{
	return m_hovered;
}

void UI_Connector::setHovered(bool hovered)
{
	bool bDifferent = (m_hovered != hovered);

	m_hovered = hovered;

	// Only repaint when needed
	if (bDifferent)
	{
		repaint();
	}
}

const UI_ConnectorStart *UI_Connector::start() const
{
	return m_start;
}

void UI_Connector::setStart(UI_ConnectorStart *start)
{
	m_start = start;
}

const UI_ConnectorEnd *UI_Connector::end() const
{
	return m_end;
}

void UI_Connector::setEnd(UI_ConnectorEnd *end)
{
	m_end = end;
}

/*
void UI_Connector::setNetwork(UI_NetworkContainer *net)
{
	m_network = net;
}*/

void UI_Connector::paintEvent(QPaintEvent *e)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QVector<QPointF> points;

	points.append(m_start->center());

	ConnectFlags flagsStart = m_start->connectFlags();
	ConnectFlags flagsEnd = ConnectFlags::None;

	if (m_end != nullptr)
		flagsEnd = m_end->connectFlags();

	if (flagsStart & ConnectFlags::OffsetRight)
	{
		points.append(m_start->center() + QPointF(CONNECTOR_OFFSET, 0));
	}
	else if (flagsStart & ConnectFlags::OffsetLeft)
	{
		points.append(m_start->center() + QPointF(-CONNECTOR_OFFSET, 0));
	}
	else if (flagsStart & ConnectFlags::OffsetDown)
	{
		points.append(m_start->center() + QPointF(0, CONNECTOR_OFFSET));
	}
	else if (flagsStart & ConnectFlags::OffsetUp)
	{
		points.append(m_start->center() + QPointF(0, -CONNECTOR_OFFSET));
	}

	QPointF &lastPoint = points[points.size() - 1];

	QPointF targetPoint;

	// If no end is in sight, we go towards the mouse
	if (m_end == nullptr)
	{
		targetPoint = mapFromGlobal(QCursor::pos());
	}
	else
		targetPoint = m_end->center();

	// If we can't go straight to the target, then we will bend the line towards it
	//if (!m_network->isLineClear(QLineF(lastPoint, targetPoint)))
	{
		//points.append(QPointF(lastPoint.x(), m_network->stateHeight(flagsStart)));
	}

	QPointF targetOffset = targetPoint;

	if (flagsEnd & ConnectFlags::OffsetLeft)
	{
		targetOffset = targetPoint + QPointF(-CONNECTOR_OFFSET, 0);
	}
	else if (flagsEnd & ConnectFlags::OffsetRight)
	{
		targetOffset = targetPoint + QPointF(CONNECTOR_OFFSET, 0);
	}
	else if (flagsEnd & ConnectFlags::OffsetUp)
	{
		targetOffset = targetPoint + QPointF(0, -CONNECTOR_OFFSET);
	}
	else if (flagsEnd & ConnectFlags::OffsetDown)
	{
		targetOffset = targetPoint + QPointF(0, CONNECTOR_OFFSET);
	}

	//if (!m_network->isLineClear(QLineF(lastPoint, targetPoint)))
	{
		//points.append(QPointF(targetOffset.x(), m_network->stateHeight(flagsStart)));
	}

	points.append(targetOffset);

	if (targetOffset != targetPoint)
		points.append(targetPoint);

	painter.setPen(QPen(QColor(0, 0, 0, 255), CONNECTOR_SIZE));
	painter.setBrush(Qt::BrushStyle::NoBrush);

	painter.drawLines(points);

	painter.end();
}

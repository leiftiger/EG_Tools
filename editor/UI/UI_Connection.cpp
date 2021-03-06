#include "UI_Connection.h"

#pragma GCC diagnostic ignored "-Wunused-parameter" // Ignore g++ warnings about this, since it's not much we can do about Qt's events

UI_Connection::UI_Connection(QWidget *parent)
	: QWidget(parent)
{
	// TODO: Better way of rendering over entire network container
	setFixedSize(parent->size()*100);

	this->setAttribute(Qt::WA_InputMethodEnabled, true);
	this->setAttribute(Qt::WA_InputMethodTransparent, true);
	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	setMouseTracking(true);
}

UI_Connection::~UI_Connection()
{
	if (m_network != nullptr)
		m_network->clearMyMemory(m_start->connectFlags(), this);
}

bool UI_Connection::hovered() const
{
	return m_hovered;
}

void UI_Connection::setHovered(bool hovered)
{
	bool bDifferent = (m_hovered != hovered);

	m_hovered = hovered;

	// Only repaint when needed
	if (bDifferent)
	{
		if (hovered)
			raise();

		update();
	}
}

bool UI_Connection::highlighted() const
{
	return m_highlighted;
}

void UI_Connection::setHighlighted(bool highlighted)
{
	bool bDifferent = (m_highlighted != highlighted);

	m_highlighted = highlighted;

	// Only repaint when needed
	if (bDifferent)
	{
		if (highlighted)
			raise();

		update();
	}
}

const UI_ConnectorStart *UI_Connection::start() const
{
	return m_start;
}

void UI_Connection::setStart(UI_ConnectorStart *start)
{
	m_start = start;
}

const UI_ConnectorEnd *UI_Connection::end() const
{
	return m_end;
}

void UI_Connection::setEnd(UI_ConnectorEnd *end)
{
	m_end = end;
}

bool UI_Connection::connecting() const
{
	return m_connecting;
}

void UI_Connection::setConnecting(bool connecting)
{
	m_connecting = connecting;
	m_highlighted = false;

	this->setAttribute(Qt::WA_TransparentForMouseEvents, !connecting);
}

void UI_Connection::setNetwork(NetworkContainerProxy *network)
{
	m_network = network;
}

void UI_Connection::paintEvent(QPaintEvent *e)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QVector<QPoint> points;

	points.append(mapFromGlobal(m_start->mapToGlobal(m_start->center())));

	ConnectFlags flagsStart = m_start->connectFlags();
	ConnectFlags flagsEnd = ConnectFlags::None;

	if (m_end != nullptr)
		flagsEnd = m_end->connectFlags();

	int startOffset = m_start->connectorOffset();

	if (flagsStart & ConnectFlags::OffsetRight)
	{
		points.append(mapFromGlobal(m_start->mapToGlobal(m_start->center() + QPoint(startOffset, 0))));
	}
	else if (flagsStart & ConnectFlags::OffsetLeft)
	{
		points.append(mapFromGlobal(m_start->mapToGlobal(m_start->center() + QPoint(-startOffset, 0))));
	}
	else if (flagsStart & ConnectFlags::OffsetDown)
	{
		points.append(mapFromGlobal(m_start->mapToGlobal(m_start->center() + QPoint(0, startOffset))));
	}
	else if (flagsStart & ConnectFlags::OffsetUp)
	{
		points.append(mapFromGlobal(m_start->mapToGlobal(m_start->center() + QPoint(0, -startOffset))));
	}

	QPoint &lastPoint = points[points.size() - 1];

	QPoint targetPoint;

	// If no end is in sight, we go towards the mouse
	if (m_end == nullptr)
	{
		targetPoint = mapFromGlobal(QCursor::pos());
	}
	else
		targetPoint = mapFromGlobal(m_end->mapToGlobal(m_end->center()));

	bool bLineClear = false;

	// Never allow direct connections from above connectors (threads)
	if (flagsStart & ConnectFlags::Above)
		bLineClear = false;
	else
		bLineClear = m_network->isLineClear(QLine(lastPoint, targetPoint));

	// If we can't go straight to the target, then we will bend the line towards it
	if (!bLineClear)
	{
		points.append(lastPoint);
		points.append(QPoint(lastPoint.x(), m_network->stateHeight(flagsStart, this)));
	}

	QPoint targetOffset = targetPoint;

	if (flagsEnd & ConnectFlags::OffsetLeft)
	{
		targetOffset = targetPoint + QPoint(-CONNECTOR_OFFSET, 0);
	}
	else if (flagsEnd & ConnectFlags::OffsetRight)
	{
		targetOffset = targetPoint + QPoint(CONNECTOR_OFFSET, 0);
	}
	else if (flagsEnd & ConnectFlags::OffsetUp)
	{
		targetOffset = targetPoint + QPoint(0, -CONNECTOR_OFFSET);
	}
	else if (flagsEnd & ConnectFlags::OffsetDown)
	{
		targetOffset = targetPoint + QPoint(0, CONNECTOR_OFFSET);
	}

	if (!bLineClear)
	{
		points.append(points[points.size() - 1]);
		points.append(QPoint(targetOffset.x(), m_network->stateHeight(flagsStart, this)));
		points.append(QPoint(targetOffset.x(), m_network->stateHeight(flagsStart, this)));
		points.append(targetOffset);
	}
	else
	{
		points.append(lastPoint);
		points.append(targetOffset);
	}

	if (targetOffset != targetPoint)
	{
		points.append(targetOffset);
		points.append(targetPoint);
	}

	if (m_highlighted)
		painter.setPen(QPen(CONNECTOR_COLOR_HIGHLIGHTED, CONNECTION_SIZE));
	else if (m_hovered || m_connecting)
		painter.setPen(QPen(CONNECTOR_COLOR_HOVERED, CONNECTION_SIZE));
	else
		painter.setPen(QPen(CONNECTOR_COLOR_DEFAULT, CONNECTION_SIZE));

	painter.setBrush(Qt::BrushStyle::NoBrush);

	painter.drawLines(points);

	painter.end();
}

void UI_Connection::mouseMoveEvent(QMouseEvent *e)
{
	if (!m_connecting)
		return;

	setAttribute(Qt::WA_TransparentForMouseEvents, true);

	QWidget *widget = QApplication::widgetAt(QCursor::pos());

	setAttribute(Qt::WA_TransparentForMouseEvents, false);

	// Mouse is outside of window
	if (widget == nullptr)
		return;

	if (typeid(*widget) == typeid(UI_ConnectorEnd))
	{
		m_end = (UI_ConnectorEnd*)widget;
	}
	else
	{
		m_end = nullptr;
	}

	update();
}

void UI_Connection::mousePressEvent(QMouseEvent *e)
{
	// Cancel connect operation
	if (e->button() == Qt::MouseButton::RightButton)
	{
		m_start->setConnection(nullptr);

		deleteLater();
	}
	else if (e->button() == Qt::MouseButton::LeftButton)
	{
		if (m_end != nullptr)
		{
			setConnecting(false);

			update();

			emit m_start->establishTransition();
		}
	}
}

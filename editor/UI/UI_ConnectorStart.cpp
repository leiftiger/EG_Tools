#include "UI_ConnectorStart.h"

UI_ConnectorStart::UI_ConnectorStart(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// Ensure widget is redrawn on hover
	this->setAttribute(Qt::WA_Hover);

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(openContextMenu(const QPoint&)));
}

UI_ConnectorStart::~UI_ConnectorStart()
{
	// When the connector is deleted, we have to delete the connection as well
	if (m_connector != nullptr)
	{
		delete m_connector;
	}
}

const UI_Connector *UI_ConnectorStart::connector() const
{
	return m_connector;
}

void UI_ConnectorStart::setConnector(UI_Connector *connector)
{
	m_connector = connector;
}

void UI_ConnectorStart::deleteConnector()
{
	m_connector->deleteLater();
	m_connector = nullptr;
}

ConnectFlags UI_ConnectorStart::connectFlags()
{
	return m_connectFlags;
}

void UI_ConnectorStart::setConnectFlags(ConnectFlags flags)
{
	m_connectFlags = flags;
}

void UI_ConnectorStart::paintEvent(QPaintEvent *e)
{
	QStyleOption option = QStyleOption();
	option.initFrom(this);

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Connector bounds

	painter.setPen(QPen(QColor(0, 0, 0, 255), 2));
	painter.setBrush(Qt::BrushStyle::NoBrush);

	painter.drawEllipse(QRectF(rect().center() - QPointF(rect().width() * CONNECTOR_SCALE * 0.5f, rect().height() * CONNECTOR_SCALE * 0.5f), QSizeF(rect().size().width() * CONNECTOR_SCALE, rect().size().height() * CONNECTOR_SCALE)));

	// Start connectors have a filled circle in the middle to signify that they spawn transitions

	painter.setPen(Qt::PenStyle::NoPen);
	painter.setBrush(QBrush(QColor(0, 0, 0, 255), Qt::BrushStyle::SolidPattern));

	painter.drawEllipse(QRectF(rect().center() - QPointF(CONNECTOR_SIZE.width() * 0.5f, CONNECTOR_SIZE.height() * 0.5f), CONNECTOR_SIZE));

	// Show that this connector is hovered
	if ((option.state & QStyle::State_MouseOver) > 0)
	{
		painter.setPen(QPen(QColor(0, 0, 0, 192), 0.5f));
		painter.setBrush(Qt::BrushStyle::NoBrush);

		painter.drawEllipse(QRectF(rect().center() - QPointF(CONNECTOR_SIZE.width() * 0.75f, CONNECTOR_SIZE.height() * 0.75f), QSizeF(CONNECTOR_SIZE.width() * 1.5f, CONNECTOR_SIZE.height() * 1.5f)));

		if (m_connector != nullptr)
			m_connector->setHovered(true);
	}
	else
	{
		if (m_connector != nullptr)
			m_connector->setHovered(false);
	}

	painter.end();
}

void UI_ConnectorStart::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton && !m_readOnly)
	{
		if (m_connector != nullptr)
		{
			m_connector->setEnd(nullptr);
		}
		else
		{
			emit createNewConnector();
		}

		m_connector->setConnecting(true);
	}
}

QPoint UI_ConnectorStart::center() const
{
	return QPoint(width() / 2, height() / 2);
}

int UI_ConnectorStart::connectorOffset() const
{
	return m_connectorOffset;
}

void UI_ConnectorStart::setConnectorOffset(int offset)
{
	m_connectorOffset = offset;
}

void UI_ConnectorStart::updateConnector()
{
	if (m_connector != nullptr)
		m_connector->update();
}

void UI_ConnectorStart::setReadOnly(bool readonly)
{
	m_readOnly = readonly;
}

void UI_ConnectorStart::highlightConnector()
{
	m_connector->setHovered(false);
	m_connector->setHighlighted(!m_connector->highlighted());
}

void UI_ConnectorStart::jumpToEndConnector()
{
	m_connector->setHovered(false);

	// Highlight temporarily so it's easier to see
	if (!m_connector->highlighted())
	{
		m_connector->setHighlighted(true);
		QTimer::singleShot(2000, this, SLOT(highlightConnector()));
	}

	emit requestJumpToWidget(m_connector->end()->parentWidget());
}

void UI_ConnectorStart::openContextMenu(const QPoint &pos)
{
	if (m_connector == nullptr)
		return;

	QMenu contextMenu(tr("Context menu"), this);

	QAction actionHighlight("Highlight", this);

	connect(&actionHighlight, SIGNAL(triggered()), this, SLOT(highlightConnector()));

	QAction actionJump("Jump to state", this);

	if (m_connector->end() == nullptr)
		actionJump.setEnabled(false);

	connect(&actionJump, SIGNAL(triggered()), this, SLOT(jumpToEndConnector()));

	contextMenu.addAction(&actionHighlight);
	contextMenu.addAction(&actionJump);

	contextMenu.exec(mapToGlobal(pos));
}

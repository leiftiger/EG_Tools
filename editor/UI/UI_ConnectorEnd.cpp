#include "UI_ConnectorEnd.h"

UI_ConnectorEnd::UI_ConnectorEnd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

UI_ConnectorEnd::~UI_ConnectorEnd()
{
}

ConnectFlags UI_ConnectorEnd::connectFlags()
{
	return m_connectFlags;
}

void UI_ConnectorEnd::setConnectFlags(ConnectFlags flags)
{
	m_connectFlags = flags;
}


void UI_ConnectorEnd::paintEvent(QPaintEvent *e)
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

	// End connectors have a hollow circle in the middle to signify that they receive transitions

	painter.setPen(QPen(QColor(0, 0, 0, 255), 1));
	painter.setBrush(Qt::BrushStyle::NoBrush);

	painter.drawEllipse(QRectF(rect().center() - QPointF(CONNECTOR_SIZE.width() * 0.5f, CONNECTOR_SIZE.height() * 0.5f), CONNECTOR_SIZE));

	painter.end();
}

QPointF UI_ConnectorEnd::center() const
{
	return QPointF(x() + width() * 0.5f, y() + height() * 0.5f);
}

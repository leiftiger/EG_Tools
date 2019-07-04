#include "UI_ConnectorStart.h"

UI_ConnectorStart::UI_ConnectorStart(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// Ensure widget is redrawn on hover
	this->setAttribute(Qt::WA_Hover);
}

UI_ConnectorStart::~UI_ConnectorStart()
{
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
	}

	painter.end();
}

#include "UI_ConnectorBranch.h"

UI_ConnectorBranch::UI_ConnectorBranch(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_newTransitionConnector = new UI_ConnectorStart(this);
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

	QVector<QPointF> points;

	painter.drawLines(points);

	painter.end();
}

#include "UI_NetworkTransition.h"

UI_NetworkTransition::UI_NetworkTransition(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_connector = new UI_ConnectorStart(this);
	m_connector->setFixedSize(21, 21);
}

UI_NetworkTransition::~UI_NetworkTransition()
{
}

std::string UI_NetworkTransition::interpret()
{
	return "<TODO>";
}

void UI_NetworkTransition::layout()
{
	ui.label->setText(QString::fromStdString(interpret()));

	// Height is fixed so the connector fits the transition container
	int height = ui.label->height();

	ui.label->adjustSize();

	ui.label->setFixedHeight(height);

	m_connector->move(ui.label->x() + ui.label->width() + LABEL_PADDING, ui.label->y() + ui.label->height()/2 - m_connector->height()/2);

	adjustSize();
}

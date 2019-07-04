#pragma once

#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorStart.h"

class UI_ConnectorStart : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorStart(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorStart();

	virtual void paintEvent(QPaintEvent *e) override;

private:
	Ui::UI_ConnectorStart ui;

	const QSizeF CONNECTOR_SIZE = QSizeF(5, 5);
	const float CONNECTOR_SCALE = 0.6f;
	
};

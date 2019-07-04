#pragma once

#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorEnd.h"

class UI_ConnectorEnd : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorEnd(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorEnd();

	virtual void paintEvent(QPaintEvent *e) override;

private:
	Ui::UI_ConnectorEnd ui;

	const QSizeF CONNECTOR_SIZE = QSizeF(5, 5);
	const float CONNECTOR_SCALE = 0.6f;
};

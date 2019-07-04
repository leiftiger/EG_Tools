#pragma once

#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorEnd.h"

#include "UI_ConnectFlags.h"

class UI_ConnectorEnd : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorEnd(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorEnd();

	// Flags that connections to this state should fulfill
	ConnectFlags connectFlags();
	// Set flags that connections to this state should fulfill
	void setConnectFlags(ConnectFlags flags);

	virtual void paintEvent(QPaintEvent *e) override;

	// Gets the center of this connector
	QPointF center() const;

private:
	Ui::UI_ConnectorEnd ui;

	const QSizeF CONNECTOR_SIZE = QSizeF(5, 5);
	const double CONNECTOR_SCALE = 0.6f;

	ConnectFlags m_connectFlags = ConnectFlags::OffsetLeft;
};

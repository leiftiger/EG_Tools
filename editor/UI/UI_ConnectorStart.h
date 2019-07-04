#pragma once

#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorStart.h"
#include "UI_ConnectFlags.h"

// Forward declared
class UI_Connector;

class UI_ConnectorStart : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorStart(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorStart();

	const UI_Connector *connector() const;
	void setConnector(UI_Connector *connector);

	// Flags that connections from this state should fulfill
	ConnectFlags connectFlags();
	// Set flags that connections from this state should fulfill
	void setConnectFlags(ConnectFlags flags);

	virtual void paintEvent(QPaintEvent *e) override;

	// Gets the center of this connector
	QPointF center() const;

private:
	Ui::UI_ConnectorStart ui;

	const QSizeF CONNECTOR_SIZE = QSizeF(5, 5);
	const double CONNECTOR_SCALE = 0.6f;

	UI_Connector *m_connector = nullptr;

	ConnectFlags m_connectFlags = ConnectFlags::OffsetRight;
	
};

#include "UI_Connector.h"
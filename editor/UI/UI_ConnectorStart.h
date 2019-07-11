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

	virtual void mousePressEvent(QMouseEvent *event) override;

	// Gets the center of this connector
	QPoint center() const;

	int connectorOffset() const;

	void setConnectorOffset(int offset);

	// Establish a transition from the connector's current connection
	void establishTransition();

private:
	Ui::UI_ConnectorStart ui;

	const QSizeF CONNECTOR_SIZE = QSizeF(5, 5);
	const double CONNECTOR_SCALE = 0.6f;

	int m_connectorOffset = CONNECTOR_OFFSET;

	UI_Connector *m_connector = nullptr;

	ConnectFlags m_connectFlags = ConnectFlags::OffsetRight;

public:
signals:
	void createNewConnector();
	
};

#include "UI_Connector.h"
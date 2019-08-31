#pragma once

#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QEvent>
#include <QWidget>
#include "GeneratedFiles/ui_UI_ConnectorStart.h"
#include "UI_ConnectFlags.h"

// Forward declared
class UI_Connection;

class UI_ConnectorStart : public QWidget
{
	Q_OBJECT

public:
	UI_ConnectorStart(QWidget *parent = Q_NULLPTR);
	~UI_ConnectorStart();

	const UI_Connection *connection() const;
	void setConnection(UI_Connection *connection);

	void deleteConnection();

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

	// Repaints the connection attached to this connector point if it exists
	void updateConnector();

	// Allows the connector to be hovered & highlighted, but not connectable
	void setReadOnly(bool readonly);

	// Check if the connector can only be hovered & highlighted, but not connectable
	bool isReadOnly() const;

private:
	Ui::UI_ConnectorStart ui;

	const QSizeF CONNECTOR_SIZE = QSizeF(5, 5);
	const double CONNECTOR_SCALE = 0.6f;

	int m_connectorOffset = CONNECTOR_OFFSET;

	UI_Connection *m_connection = nullptr;

	ConnectFlags m_connectFlags = ConnectFlags::OffsetRight;

	bool m_readOnly = false;

protected slots:
	void openContextMenu(const QPoint &pos);
	void highlightConnector();
	void jumpToEndConnector();

public:
signals:
	void createNewConnector();

	// Establish a transition from the connector's current connection
	void establishTransition();

	void requestJumpToWidget(QWidget *widget);
	
};

#include "UI_Connection.h"
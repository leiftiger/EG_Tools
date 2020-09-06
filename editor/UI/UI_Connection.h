#pragma once

#include <QWidget>
#include <QPaintEvent>

#include "UI_ConnectorStart.h"
#include "UI_ConnectorEnd.h"

#include "UI_ConnectFlags.h"

#include "NetworkContainerProxy.h"

// Forward declared
class UI_ConnectorStart;
class UI_ConnectorEnd;
class NetworkContainerProxy;

class UI_Connection : public QWidget
{
	Q_OBJECT

public:
	UI_Connection(QWidget *parent = Q_NULLPTR);
	~UI_Connection();

	bool hovered() const;
	void setHovered(bool hovered);

	bool highlighted() const;
	void setHighlighted(bool highlighted);

	const UI_ConnectorStart *start() const;
	void setStart(UI_ConnectorStart *start);

	const UI_ConnectorEnd *end() const;
	void setEnd(UI_ConnectorEnd *end);

	bool connecting() const;
	void setConnecting(bool connecting);

	void setNetwork(NetworkContainerProxy *network);

	virtual void paintEvent(QPaintEvent *e) override;

	virtual void mouseMoveEvent(QMouseEvent *e) override;

	virtual void mousePressEvent(QMouseEvent *e) override;

private:

	bool m_hovered = false;
	bool m_highlighted = false;
	bool m_connecting = false;

	UI_ConnectorStart *m_start = nullptr;
	UI_ConnectorEnd *m_end = nullptr;

	NetworkContainerProxy *m_network;

	const QColor CONNECTOR_COLOR_DEFAULT = QColor(0, 0, 0, 255);
	const QColor CONNECTOR_COLOR_HOVERED = QColor(255, 128, 0, 255);
	const QColor CONNECTOR_COLOR_HIGHLIGHTED = QColor(225, 91, 255, 255);
};

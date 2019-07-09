#pragma once

#include <QWidget>
#include <QPaintEvent>

#include "GeneratedFiles/ui_UI_Connector.h"

#include "UI_ConnectorStart.h"
#include "UI_ConnectorEnd.h"

#include "UI_ConnectFlags.h"

#include "NetworkContainerProxy.h"

// Forward declared
class UI_ConnectorStart;
class UI_ConnectorEnd;
class NetworkContainerProxy;

class UI_Connector : public QWidget
{
	Q_OBJECT

public:
	UI_Connector(QWidget *parent = Q_NULLPTR);
	~UI_Connector();

	bool hovered() const;
	void setHovered(bool hovered);

	const UI_ConnectorStart *start() const;
	void setStart(UI_ConnectorStart *start);

	const UI_ConnectorEnd *end() const;
	void setEnd(UI_ConnectorEnd *end);

	void setNetwork(NetworkContainerProxy *network);

	virtual void paintEvent(QPaintEvent *e) override;

private:
	Ui::UI_Connector ui;

	bool m_hovered = false;

	UI_ConnectorStart *m_start = nullptr;
	UI_ConnectorEnd *m_end = nullptr;

	NetworkContainerProxy *m_network;
};

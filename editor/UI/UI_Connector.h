#pragma once

#include <QWidget>
#include "GeneratedFiles/ui_UI_Connector.h"

#include "UI_ConnectorStart.h"
#include "UI_ConnectorEnd.h"

#include "UI_ConnectFlags.h"

// Forward declared
class UI_ConnectorStart;
class UI_ConnectorEnd;

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

	//void setNetwork(UI_NetworkContainer *net);


	virtual void paintEvent(QPaintEvent *e) override;

private:
	Ui::UI_Connector ui;

	bool m_hovered = false;

	UI_ConnectorStart *m_start = nullptr;
	UI_ConnectorEnd *m_end = nullptr;

	// Commented due to insane circular dependency if trying to access the network container
	//UI_NetworkContainer *m_network = nullptr;

	// How large the connector lines should be
	const double CONNECTOR_SIZE = 4;

	// How long the connector offset to the left or right should be
	const double CONNECTOR_OFFSET = 32;
};

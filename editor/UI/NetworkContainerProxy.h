#pragma once

#include <QLine>

#include <vector>
#include <unordered_map>

#include "UI_ConnectFlags.h"
#include "UI_Connector.h"

// Forward declared
class UI_Connector;

// Class for passing values between the network container and other
// elements without risking insane circular dependencies during compilation
class NetworkContainerProxy
{
private:
	int m_upperHeight;

	int m_lowerHeight;

	int m_maxHeightOffsetAbove = -CONNECTOR_MARGIN - CONNECTOR_SIZE;
	int m_maxHeightOffsetBelow = -CONNECTOR_MARGIN - CONNECTOR_SIZE;

	std::unordered_map<const UI_Connector*, int> m_heightOffsetsAbove;
	std::unordered_map<const UI_Connector*, int> m_heightOffsetsBelow;

	// Spaces occupied by states
	std::vector<QRect> m_stateSpaces;

public:
	NetworkContainerProxy();
	~NetworkContainerProxy();

	// How far from the states that transition connection lines should go
	static const int CONNECTOR_HEIGHT_OFFSET = 15;

	// When connectors share path, they should be at least this far away from each other
	static const int CONNECTOR_MARGIN = 2;

	void setUpperHeight(int height);

	void setLowerHeight(int height);

	void setStateSpaces(std::vector<QRect> stateSpaces);

	// Returns true if the line avoids colliding with any states
	bool isLineClear(const QLine &line) const;

	// Gets the height that is above or below the states according to the used flags
	int stateHeight(ConnectFlags flags, const UI_Connector* connector);

};


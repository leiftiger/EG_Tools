#include "NetworkContainerProxy.h"



NetworkContainerProxy::NetworkContainerProxy()
{
}


NetworkContainerProxy::~NetworkContainerProxy()
{
}

void NetworkContainerProxy::setUpperHeight(int height)
{
	m_upperHeight = height - CONNECTOR_HEIGHT_OFFSET;
}

void NetworkContainerProxy::setLowerHeight(int height)
{
	m_lowerHeight = height + CONNECTOR_HEIGHT_OFFSET;
}

bool NetworkContainerProxy::isLineClear(const QLine &line) const
{
	return false;
}

int NetworkContainerProxy::stateHeight(ConnectFlags flags, const UI_Connector* connector)
{
	int offset = 0;

	std::unordered_map<const UI_Connector*, int> *offsetList = &m_heightOffsetsBelow;
	int *maxOffset = &m_maxHeightOffsetBelow;

	if (flags & ConnectFlags::Above)
	{
		offsetList = &m_heightOffsetsAbove;
		maxOffset = &m_maxHeightOffsetAbove;
	}

	if (offsetList->find(connector) != offsetList->end())
	{
		offset = offsetList->operator[](connector);
	}
	else
	{
		offset = offsetList->operator[](connector) = *maxOffset += CONNECTOR_SIZE + CONNECTOR_MARGIN;
	}

	if (flags & ConnectFlags::Above)
		return m_upperHeight - offset;

	return m_lowerHeight + offset;
}

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

void NetworkContainerProxy::setStateSpaces(std::vector<QRect> stateSpaces)
{
	m_stateSpaces = stateSpaces;
}

bool NetworkContainerProxy::isLineClear(const QLine &arbitraryLine) const
{
	QLine line = arbitraryLine;

	// No way to connect directly backwards!
	if (line.x1() > line.x2())
		return false;

	for (const QRect &rect : m_stateSpaces)
	{
		if ((line.x1() <= rect.x() && (rect.x() + CONNECTOR_OFFSET + CONNECTOR_END_SIZE) <= line.x2()) || (rect.x() <= line.x1() && line.x1() <= (rect.x() + rect.width())))
			return false;
	}

	return true;
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

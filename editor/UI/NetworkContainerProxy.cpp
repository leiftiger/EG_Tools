#include "NetworkContainerProxy.h"



int NetworkContainerProxy::calculateOffset(const UI_Connector *connector, const std::unordered_map<const UI_Connector*, QLine> &connectorPositions)
{
	const QLine &curLine = connectorPositions.at(connector);

	int offset = 0;

	for (const std::pair<const UI_Connector*, QLine> &pair : connectorPositions)
	{
		// Can't overlap ourselves
		if (pair.first == connector)
			continue;

		const QLine &otherLine = pair.second;

		// Encapsulates other line
		if (curLine.x1() <= otherLine.x1() && otherLine.x1() <= curLine.x2() && curLine.x1() <= otherLine.x2() && otherLine.x2() <= curLine.x2())
		{
			offset += CONNECTOR_SIZE + CONNECTOR_MARGIN;
		}
	}

	return offset;
}

void NetworkContainerProxy::moveCollisions(std::unordered_map<const UI_Connector*, int> &offsetList, const std::unordered_map<const UI_Connector*, QLine> &connectorPositions)
{
	for (const std::pair<const UI_Connector*, int> &pair1 : offsetList)
	{
		const QLine &curLine = connectorPositions.at(pair1.first);

		for (const std::pair<const UI_Connector*, int> &pair2 : offsetList)
		{
			if (pair1.first == pair2.first)
				continue;

			const QLine &otherLine = connectorPositions.at(pair2.first);

			// If lines overlap and are at the same offset, then move one of them
			if (pair1.second == pair2.second && ((curLine.x1() <= otherLine.x1() && otherLine.x1() <= curLine.x2()) || (curLine.x1() <= otherLine.x2() && otherLine.x2() <= curLine.x2())))
			{
				// Let the shorter connections stay above the longer ones
				if ((otherLine.x2() - otherLine.x1()) > (curLine.x2() - curLine.x1()))
					offsetList[pair2.first] += CONNECTOR_SIZE + CONNECTOR_MARGIN;
				else
					offsetList[pair1.first] += CONNECTOR_SIZE + CONNECTOR_MARGIN;

				moveCollisions(offsetList, connectorPositions);
			}
		}
	}
}

NetworkContainerProxy::NetworkContainerProxy()
{
}


NetworkContainerProxy::~NetworkContainerProxy()
{
	m_destroyed = true;
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

	std::unordered_map<const UI_Connector*, QLine> *connectorPositions = &m_connectorPositionsBelow;

	if (flags & ConnectFlags::Above)
	{
		offsetList = &m_heightOffsetsAbove;
		connectorPositions = &m_connectorPositionsAbove;
	}

	QLine *line = nullptr;

	if (connectorPositions->find(connector) != connectorPositions->end())
	{
		line = &(*connectorPositions)[connector];

		QPoint end = QCursor::pos();

		if (connector->end() != nullptr)
		{
			end = connector->mapFromGlobal(connector->end()->mapToGlobal(connector->end()->center()));
		}

		if (line->p1() != end && line->p2() != end)
		{
			line = nullptr;
		}
	}

	if (line != nullptr)
	{
		offset = (*offsetList)[connector];
	}
	else
	{
		QPoint end = connector->mapFromGlobal(QCursor::pos());

		if (connector->end() != nullptr)
		{
			end = connector->mapFromGlobal(connector->end()->mapToGlobal(connector->end()->center()));
		}

		(*connectorPositions)[connector] = QLine(connector->mapFromGlobal(connector->start()->mapToGlobal(connector->start()->center()+QPoint(connector->start()->connectorOffset(), 0))), end);

		if ((*connectorPositions)[connector].x1() > (*connectorPositions)[connector].x2())
			(*connectorPositions)[connector] = QLine((*connectorPositions)[connector].p2(), (*connectorPositions)[connector].p1());

		offset = (*offsetList)[connector] = calculateOffset(connector, *connectorPositions);

		for (const std::pair<const UI_Connector*, int> &pair : *offsetList)
		{
			if (pair.first == connector)
				continue;

			(*offsetList)[pair.first] = calculateOffset(pair.first, *connectorPositions);
		}

		moveCollisions(*offsetList, *connectorPositions);
	}

	if (flags & ConnectFlags::Above)
		return m_upperHeight - offset;

	return m_lowerHeight + offset;
}

void NetworkContainerProxy::clearMyMemory(ConnectFlags flags, const UI_Connector *connector)
{
	if (m_destroyed)
		return;

	std::unordered_map<const UI_Connector*, int> *offsetList = &m_heightOffsetsBelow;

	std::unordered_map<const UI_Connector*, QLine> *connectorPositions = &m_connectorPositionsBelow;

	if (flags & ConnectFlags::Above)
	{
		offsetList = &m_heightOffsetsAbove;
		connectorPositions = &m_connectorPositionsAbove;
	}

	offsetList->erase(connector);
	connectorPositions->erase(connector);

	for (const std::pair<const UI_Connector*, int> &pair : *offsetList)
	{
		(*offsetList)[pair.first] = calculateOffset(pair.first, *connectorPositions);
	}

	moveCollisions(*offsetList, *connectorPositions);
}

void NetworkContainerProxy::recalculateHeights()
{
	std::unordered_map<const UI_Connector *, QLine> oldAbove = m_connectorPositionsAbove;
	std::unordered_map<const UI_Connector *, QLine> oldBelow = m_connectorPositionsBelow;

	m_connectorPositionsAbove.clear();
	m_connectorPositionsBelow.clear();

	m_heightOffsetsAbove.clear();
	m_heightOffsetsBelow.clear();

	for (std::pair<const UI_Connector * const, QLine> &pair : oldAbove)
	{
		stateHeight(ConnectFlags::Above, pair.first);
	}

	for (std::pair<const UI_Connector * const, QLine> &pair : oldBelow)
	{
		stateHeight(ConnectFlags::None, pair.first);
	}
}

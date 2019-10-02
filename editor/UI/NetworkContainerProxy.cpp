#include "NetworkContainerProxy.h"



int NetworkContainerProxy::calculateOffset(const UI_Connection *connection, const std::unordered_map<const UI_Connection*, QLine> &connectorPositions)
{
	const QLine &curLine = connectorPositions.at(connection);

	int offset = 0;

	for (const std::pair<const UI_Connection*, QLine> &pair : connectorPositions)
	{
		// Can't overlap ourselves
		if (pair.first == connection)
			continue;

		const QLine &otherLine = pair.second;

		// Encapsulates other line
		if (curLine.x1() <= otherLine.x1() && otherLine.x1() <= curLine.x2() && curLine.x1() <= otherLine.x2() && otherLine.x2() <= curLine.x2())
		{
			offset += CONNECTION_SIZE + CONNECTOR_MARGIN;
		}
	}

	return offset;
}

void NetworkContainerProxy::moveCollisions(std::unordered_map<const UI_Connection*, int>::iterator itStart, std::unordered_map<const UI_Connection*, int>::iterator itEnd, const std::unordered_map<const UI_Connection*, QLine> &connectorPositions)
{
	// TODO: Optimize and improve this, this is extremely slow with large number of connections
	for (std::unordered_map<const UI_Connection*, int>::iterator it = itStart; it != itEnd; it++)
	{
		const std::pair<const UI_Connection*, int> &pair1 = *it;

		const QLine &curLine = connectorPositions.at(pair1.first);

		for (std::unordered_map<const UI_Connection*, int>::iterator it2 = it; it2 != itEnd; it2++)
		{
			const std::pair<const UI_Connection*, int> &pair2 = *it2;

			if (pair1.first == pair2.first)
				continue;

			const QLine &otherLine = connectorPositions.at(pair2.first);

			// If lines overlap and are at the same offset, then move one of them
			if (pair1.second == pair2.second && ((curLine.x1() <= otherLine.x1() && otherLine.x1() <= curLine.x2()) || (curLine.x1() <= otherLine.x2() && otherLine.x2() <= curLine.x2())))
			{
				// Let the shorter connections stay above the longer ones
				if ((otherLine.x2() - otherLine.x1()) >= (curLine.x2() - curLine.x1()))
					(*it2).second += CONNECTION_SIZE + CONNECTOR_MARGIN;
				else
					(*it).second += CONNECTION_SIZE + CONNECTOR_MARGIN;

				moveCollisions(itStart, itEnd, connectorPositions);
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

int NetworkContainerProxy::stateHeight(ConnectFlags flags, const UI_Connection* connection)
{
	int offset = 0;

	std::unordered_map<const UI_Connection*, int> *offsetList = &m_heightOffsetsBelow;

	std::unordered_map<const UI_Connection*, QLine> *connectorPositions = &m_connectorPositionsBelow;

	if (flags & ConnectFlags::Above)
	{
		offsetList = &m_heightOffsetsAbove;
		connectorPositions = &m_connectorPositionsAbove;
	}

	QLine *line = nullptr;

	if (connectorPositions->find(connection) != connectorPositions->end())
	{
		line = &(*connectorPositions)[connection];

		QPoint end = QCursor::pos();

		if (connection->end() != nullptr)
		{
			end = connection->mapFromGlobal(connection->end()->mapToGlobal(connection->end()->center()));
		}

		if (line->p1() != end && line->p2() != end)
		{
			line = nullptr;
		}
	}

	if (line != nullptr)
	{
		offset = (*offsetList)[connection];
	}
	else
	{
		QPoint end = connection->mapFromGlobal(QCursor::pos());

		if (connection->end() != nullptr)
		{
			end = connection->mapFromGlobal(connection->end()->mapToGlobal(connection->end()->center()));
		}

		(*connectorPositions)[connection] = QLine(connection->mapFromGlobal(connection->start()->mapToGlobal(connection->start()->center()+QPoint(connection->start()->connectorOffset(), 0))), end);

		if ((*connectorPositions)[connection].x1() > (*connectorPositions)[connection].x2())
			(*connectorPositions)[connection] = QLine((*connectorPositions)[connection].p2(), (*connectorPositions)[connection].p1());

		offset = (*offsetList)[connection] = calculateOffset(connection, *connectorPositions);

		for (const std::pair<const UI_Connection*, int> &pair : *offsetList)
		{
			if (pair.first == connection)
				continue;

			(*offsetList)[pair.first] = calculateOffset(pair.first, *connectorPositions);
		}

		moveCollisions(offsetList->begin(), offsetList->end(), *connectorPositions);
	}

	if (flags & ConnectFlags::Above)
		return m_upperHeight - offset;

	return m_lowerHeight + offset;
}

void NetworkContainerProxy::clearMyMemory(ConnectFlags flags, const UI_Connection *connection)
{
	if (m_destroyed)
		return;

	std::unordered_map<const UI_Connection*, int> *offsetList = &m_heightOffsetsBelow;

	std::unordered_map<const UI_Connection*, QLine> *connectorPositions = &m_connectorPositionsBelow;

	if (flags & ConnectFlags::Above)
	{
		offsetList = &m_heightOffsetsAbove;
		connectorPositions = &m_connectorPositionsAbove;
	}

	offsetList->erase(connection);
	connectorPositions->erase(connection);

	for (const std::pair<const UI_Connection*, int> &pair : *offsetList)
	{
		(*offsetList)[pair.first] = calculateOffset(pair.first, *connectorPositions);
	}

	moveCollisions(offsetList->begin(), offsetList->end(), *connectorPositions);
}

void NetworkContainerProxy::recalculateHeights()
{
	std::unordered_map<const UI_Connection *, QLine> oldAbove = m_connectorPositionsAbove;
	std::unordered_map<const UI_Connection *, QLine> oldBelow = m_connectorPositionsBelow;

	m_connectorPositionsAbove.clear();
	m_connectorPositionsBelow.clear();

	m_heightOffsetsAbove.clear();
	m_heightOffsetsBelow.clear();

	for (std::pair<const UI_Connection * const, QLine> &pair : oldAbove)
	{
		stateHeight(ConnectFlags::Above, pair.first);
	}

	for (std::pair<const UI_Connection * const, QLine> &pair : oldBelow)
	{
		stateHeight(ConnectFlags::None, pair.first);
	}
}

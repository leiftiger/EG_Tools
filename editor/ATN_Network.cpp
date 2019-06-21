#include "ATN_Network.h"

namespace ATN
{
	const char * const Network::typeName() const
	{
		return "TATNNetwork";
	}

	void Network::serialize(std::ostream &stream) const
	{
		Entry::serialize(stream);

		stream << this->m_resources << this->m_parameters;

		// Header for states
		stream << "ContainerID=ATNData" << std::endl;

		util::writeEntryIDs<State>(stream, "States=", this->m_states);

		// Header for threads
		stream << "ContainerID=ATNData" << std::endl;

		util::writeEntryIDs<Thread>(stream, "Threads=", this->m_threads);

		// Signify end of object
		stream << std::endl;
	}

	void Network::deserialize(std::istream &stream)
	{
		Entry::deserialize(stream);

		std::string line;

		stream >> this->m_resources >> this->m_parameters;

		util::getline(stream, line);

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		this->m_states = util::parseEntryIDs<State>(stream, "States=");

		if (line != "ContainerID=ATNData")
			throw Exception("Expected \"ContainerID=ATNData\", got \"%s\"", line);

		this->m_threads = util::parseEntryIDs<Thread>(stream, "Threads=");

		util::getline(stream, line); // blank line - we reached the end of this object
	}
}
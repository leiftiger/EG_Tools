#include "MutexCommunication.h"

namespace Mutex
{
	Client::Client(bool &clientWaiting, bool &clientAcknowledged, const bool &serverWaiting, const bool &serverAcknowledged, const bool &terminateSignal, std::queue<std::string> &queue, const double &progress) :
		m_clientWaiting(clientWaiting), m_clientAcknowledged(clientAcknowledged), m_serverWaiting(serverWaiting), m_serverAcknowledged(serverAcknowledged), m_terminate(terminateSignal), queue(queue), progress(progress)
	{

	}

	void Client::wait()
	{
		m_clientWaiting = true;

		while (!m_serverAcknowledged)
			std::this_thread::yield();

		m_clientWaiting = false;

		while (m_serverAcknowledged)
			std::this_thread::yield();
	}

	bool Client::isWaiting()
	{
		return m_serverWaiting;
	}

	void Client::acknowledge()
	{
		m_clientAcknowledged = true;

		while (m_serverWaiting)
			std::this_thread::yield();

		m_clientAcknowledged = false;
	}

	bool Client::shouldTerminate() const
	{
		return m_terminate;
	}

	Server::Server(const bool &clientWaiting, const bool &clientAcknowledged, bool &serverWaiting, bool &serverAcknowledged, const bool &terminateSignal, std::queue<std::string> &queue, double &progress) :
		m_clientWaiting(clientWaiting), m_clientAcknowledged(clientAcknowledged), m_serverWaiting(serverWaiting), m_serverAcknowledged(serverAcknowledged), m_terminate(terminateSignal), queue(queue), progress(progress)
	{

	}

	void Server::wait()
	{
		m_serverWaiting = true;

		while (!m_clientAcknowledged)
			std::this_thread::yield();

		m_serverWaiting = false;

		while (m_clientAcknowledged)
			std::this_thread::yield();
	}

	bool Server::isWaiting()
	{
		return m_serverWaiting;
	}

	void Server::acknowledge()
	{
		m_serverAcknowledged = true;

		while (m_clientWaiting)
			std::this_thread::yield();

		m_serverAcknowledged = false;
	}

	bool Server::shouldTerminate() const
	{
		return m_terminate;
	}

	Pair::Pair() : 
		m_client(Client(m_clientWaiting, m_clientAcknowledged, m_serverWaiting, m_serverAcknowledged, m_terminate, m_queue, m_progress)),
		m_server(Server(m_clientWaiting, m_clientAcknowledged, m_serverWaiting, m_serverAcknowledged, m_terminate, m_queue, m_progress))
	{

	}

	Client &Pair::client()
	{
		return m_client;
	}

	Server &Pair::server()
	{
		return m_server;
	}

	void Pair::terminate()
	{
		m_terminate = true;
	}
}

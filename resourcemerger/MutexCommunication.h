#pragma once

#include <string>
#include <queue>
#include <thread>

// Namespace for mutex classes
namespace Mutex
{
	// The client that reads from the stream provided by the server
	class Client
	{
	protected:
		bool &m_clientWaiting;
		bool &m_clientAcknowledged;

		const bool &m_serverWaiting;
		const bool &m_serverAcknowledged;

		const bool &m_terminate;

	public:

		std::queue<std::string> &queue;

		const double &progress;

		Client(bool &clientWaiting, bool &clientAcknowledged, const bool &serverWaiting, const bool &serverAcknowledged, const bool &terminateSignal, std::queue<std::string> &queue, const double &progress);

		// Blocks until the server acknowledges
		void wait();

		// Returns true if server is waiting for acknowledgement
		bool isWaiting();

		// Acknowledges server's wait
		void acknowledge();

		// Whether or not the client should terminate
		bool shouldTerminate() const;
	};

	// The server that writes to the stream read by the client
	class Server
	{
	protected:
		const bool &m_clientWaiting;
		const bool &m_clientAcknowledged;

		bool &m_serverWaiting;
		bool &m_serverAcknowledged;

		const bool &m_terminate;

	public:

		std::queue<std::string> &queue;

		double &progress;

		Server(const bool &clientWaiting, const bool &clientAcknowledged, bool &serverWaiting, bool &serverAcknowledged, const bool &terminateSignal, std::queue<std::string> &queue, double &progress);

		// Blocks until the client acknowledges
		void wait();

		// Returns true if client is waiting for acknowledgement
		bool isWaiting();

		// Acknowledges client's wait
		void acknowledge();

		// Whether or not the server should terminate
		bool shouldTerminate() const;
	};

	// A pair containing a client-server mutex structure
	class Pair
	{
	protected:
		bool m_clientWaiting;
		bool m_clientAcknowledged;

		bool m_serverWaiting;
		bool m_serverAcknowledged;

		bool m_terminate;

		std::queue<std::string> m_queue;

		double m_progress;

		Client m_client;
		Server m_server;

	public:

		Pair();

		Client &client();
		Server &server();

		void terminate();
	};
}
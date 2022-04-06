#include "pch.h"
#include "Chesster/Connections/Winsock.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	Winsock::Winsock() :
		m_WSAData{ NULL }
	{
		// Initialize Winsock
		int iResult = WSAStartup(WINSOCK_VER, &m_WSAData);
		if (iResult != Result::Success) // Ensure system supports Winsock
		{
			LOG_ERROR("WSAStartup failed with error: {0}", iResult);
			throw std::runtime_error("Unable to initialize Winsock!");
		}
	}

	Winsock::~Winsock()
	{
		WSACleanup();
	}

	bool Winsock::CreateClientSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
	{
		// Contains a sockaddr structure
		struct addrinfo* Result, Hints;

		// Prepare addrinfo
		ZeroMemory(&Hints, sizeof(Hints));
		Hints.ai_family = AF_INET;			// AF_NET for IPv4. AF_NET6 for IPv6.
		Hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		Hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.

		// Resolve the server address and port
		int iResult = getaddrinfo(ip, port, &Hints, &Result);
		if (iResult == Result::Failure)
		{
			LOG_ERROR("WINSOCK: getaddrinfo failed with error: {0}", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (struct addrinfo* ResultPtr = Result; ResultPtr != nullptr; ResultPtr = ResultPtr->ai_next)
		{
			// Create a SOCKET for connecting to server
			m_socket = socket(ResultPtr->ai_family, ResultPtr->ai_socktype, ResultPtr->ai_protocol);
			if (m_socket == INVALID_SOCKET) // Ensure that the socket is a valid socket.
			{
				LOG_ERROR("WINSOCK: socket failed with error: {0}", WSAGetLastError());
				freeaddrinfo(Result);
				return false;
			}

			// Connect to server.
			iResult = connect(m_socket, ResultPtr->ai_addr, (int)ResultPtr->ai_addrlen);
			if (iResult == SOCKET_ERROR) // Check for general errors.
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		// Not needed anymore, free the memory
		freeaddrinfo(Result);

		if (m_socket == INVALID_SOCKET)
			return false;

		return true;
	}

	bool Winsock::CreateServerSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
	{
		// Contains a sockaddr structure
		struct addrinfo* Result, Hints;

		// Prepare addrinfo
		ZeroMemory(&Hints, sizeof(Hints));	// ZeroMemory fills the hints with zeros, prepares the memory to be used
		Hints.ai_family = AF_INET;			// AF_NET for IPv4. AF_NET6 for IPv6. AF_UNSPEC for either (might cause error).
		Hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		Hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.
		Hints.ai_flags = AI_PASSIVE;		// Indicates the caller intends to use the returned socket address structure
											// in a call to the bind function.
		// Resolve the local address and port to be used by the server
		// The getaddrinfo function is used to determine the values in the sockaddr structure
		int iResult = getaddrinfo(ip, port, &Hints, &Result);
		if (iResult != Result::Success) // Error checking: ensure an address was received
		{
			LOG_ERROR("WINSOCK: getaddrinfo() failed with error: {0}", iResult);
			return false;
		}

		// Create a SOCKET for the server to listen for client connections
		m_socket = socket(Result->ai_family, Result->ai_socktype, Result->ai_protocol);
		if (m_socket == INVALID_SOCKET) // Error checking: ensure the socket is valid.
		{
			LOG_ERROR("WINSOCK: socket() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(Result);
			return false;
		}

		// For a server to accept client connections,
		// it must be bound to a network address within the system.

		// Setup the TCP listening socket
		iResult = bind(m_socket, Result->ai_addr, (int)Result->ai_addrlen);
		if (iResult == SOCKET_ERROR) // Error checking
		{
			LOG_ERROR("WINSOCK: bind() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(Result);
			closesocket(m_socket);
			return false;
		}

		// Once the bind function is called, the address information
		// returned by the getaddrinfo function is no longer needed
		freeaddrinfo(Result);

		// After the socket is bound to an IP address and port on the system, the server
		// must then listen on that IP address and port for incoming connection requests.
		if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			LOG_ERROR("WINSOCK: listen() failed with error: {0}", WSAGetLastError());
			closesocket(m_socket);
			return false;
		}

		return true;
	}

	SOCKET Winsock::AcceptClient(const SOCKET& listenSocket)
	{
		return accept(listenSocket, nullptr, nullptr);
	}
}

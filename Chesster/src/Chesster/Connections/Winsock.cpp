#include "pch.h"
#include "Chesster/Connections/Winsock.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	Winsock::Winsock() :
		m_WSAData{ NULL },
		m_SockAddr{ NULL },
		m_SockAddrSize{ sizeof(m_SockAddr) },
		m_Result{ nullptr },
		m_Ptr{ nullptr }
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
		// Shut down the socket DLL
		WSACleanup();
	}

	bool Winsock::CreateClientSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
	{
		struct addrinfo hints;

		// Prepare addrinfo
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;			// AF_NET for IPv4. AF_NET6 for IPv6.
		hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.

		// Resolve the server address and port
		int iResult = getaddrinfo(ip, port, &hints, &m_Result);
		if (iResult == Result::Failure)
		{
			LOG_ERROR("WINSOCK: getaddrinfo failed with error: {0}", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (m_Ptr = m_Result; m_Ptr != nullptr; m_Ptr = m_Ptr->ai_next)
		{
			// Create a SOCKET for connecting to server
			m_socket = socket(m_Ptr->ai_family, m_Ptr->ai_socktype, m_Ptr->ai_protocol);
			if (m_socket == INVALID_SOCKET) // Ensure that the socket is a valid socket.
			{
				LOG_ERROR("WINSOCK: socket failed with error: {0}", WSAGetLastError());
				freeaddrinfo(m_Result);
				return false;
			}

			// Connect to server.
			iResult = connect(m_socket, m_Ptr->ai_addr, (int)m_Ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) // Check for general errors.
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		// Not needed anymore, free the memory
		freeaddrinfo(m_Result);

		if (m_socket == INVALID_SOCKET)
			return false;

		return true;
	}

	bool Winsock::CreateServerSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
	{
		struct addrinfo hints;

		// Prepare addrinfo
		ZeroMemory(&hints, sizeof(hints));	// ZeroMemory fills the hints with zeros, prepares the memory to be used
		hints.ai_family = AF_INET;			// AF_NET for IPv4. AF_NET6 for IPv6. AF_UNSPEC for either (might cause error).
		hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.
		hints.ai_flags = AI_PASSIVE;		// Indicates the caller intends to use the returned socket address structure
											// in a call to the bind function.
		// Resolve the local address and port to be used by the server
		// The getaddrinfo function is used to determine the values in the sockaddr structure
		int iResult = getaddrinfo(ip, port, &hints, &m_Result);
		if (iResult != Result::Success) // Error checking: ensure an address was received
		{
			LOG_ERROR("WINSOCK: getaddrinfo() failed with error: {0}", iResult);
			return false;
		}

		// Create a SOCKET for the server to listen for client connections
		m_socket = socket(m_Result->ai_family, m_Result->ai_socktype, m_Result->ai_protocol);
		if (m_socket == INVALID_SOCKET) // Error checking: ensure the socket is valid.
		{
			LOG_ERROR("WINSOCK: socket() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(m_Result);
			return false;
		}

		// For a server to accept client connections,
		// it must be bound to a network address within the system.

		// Setup the TCP listening socket
		iResult = bind(m_socket, m_Result->ai_addr, (int)m_Result->ai_addrlen);
		if (iResult == SOCKET_ERROR) // Error checking
		{
			LOG_ERROR("WINSOCK: bind() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(m_Result);
			closesocket(m_socket);
			return false;
		}

		// Once the bind function is called, the address information
		// returned by the getaddrinfo function is no longer needed
		freeaddrinfo(m_Result);

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
		return accept(listenSocket, (sockaddr*)&m_SockAddr, &m_SockAddrSize);
	}

	void Winsock::DNSLookup(const SOCKET& m_socket)
	{
		char ServerName[NI_MAXHOST];
		char ServerPort[NI_MAXHOST];

		// Attempt to get the server's name
		int iResult = getpeername(m_socket, (sockaddr*)&m_SockAddr, &m_SockAddrSize);
		if (iResult == SOCKET_ERROR)
		{
			LOG_WARN("Unable to retrieve the server's address.");
		}
		else
		{
			if (getnameinfo((sockaddr*)&m_SockAddr, m_SockAddrSize, ServerName, NI_MAXHOST, ServerPort, NI_MAXSERV, 0) == 0)
			{
				LOG_INFO("Connected to {0} on port {1}", ServerName, ServerPort);
			}
			else // if unable to get name then show IP
			{
				inet_ntop(m_Result->ai_family, &m_SockAddr.sin_addr, ServerName, NI_MAXHOST);
				LOG_INFO("Connected to {0} on port {1}", ServerName, ntohs(m_SockAddr.sin_port));
			}
		}
	}
}

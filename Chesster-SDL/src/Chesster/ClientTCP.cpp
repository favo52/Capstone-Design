#include "pch.h"
#include "ClientTCP.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	std::string ClientTCP::m_Data{ "" };
	SOCKET ClientTCP::m_BufferSocket{ INVALID_SOCKET };

	ClientTCP::ClientTCP() :
		m_WSAData{ NULL },
		m_CommandSocket{ INVALID_SOCKET }
	{
		// Initialize Winsock
		int iResult = WSAStartup(WINSOCK_VER, &m_WSAData);
		if (iResult != Result::Success) // Ensure system supports Winsock
		{
			CHESSTER_ERROR("WSAStartup failed with error: {0}", iResult);
			throw std::runtime_error("Unable to initialize Winsock!");
		}
	}

	ClientTCP::~ClientTCP()
	{
		DisconnectSockets();

		// Shut down the socket DLL
		WSACleanup();
	}

	void ClientTCP::ConnectSockets()
	{
		// To command the camera to take the picture
		if (!ConnectSocket(m_CommandSocket, "localhost", "23"))
			CHESSTER_ERROR("Unable to connect m_CommandSocket. (IP: localhost, Port: 23)");

		DNSLookup(m_CommandSocket);

		// Prepare buffer
		char Buffer[1024]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		recv(m_CommandSocket, Buffer, BufferLen, 0);

		// To receive buffer stream of physical board's changes
		if (!ConnectSocket(m_BufferSocket, "localhost", "3000"))
			CHESSTER_ERROR("Unable to connect m_BufferSocket. (IP: localhost, Port: 3000)");

		DNSLookup(m_BufferSocket);

		// Create new thread for receiving data
		unsigned threadID{};
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientTCP::ReceiveBuffer, (void*)&*this, 0, &threadID);
	}

	void ClientTCP::DisconnectSockets()
	{
		closesocket(m_CommandSocket);
		closesocket(m_BufferSocket);
	}

	bool ClientTCP::SendCommand(const std::string command)
	{
		int iSendResult = send(m_CommandSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			CHESSTER_ERROR("SendCommand failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool ClientTCP::ConnectSocket(SOCKET& m_socket, PCSTR ip, PCSTR port)
	{
		// Prepare addrinfo
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;			// AF_NET for IPv4. AF_NET6 for IPv6.
		hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.

		// Resolve the server address and port
		int iResult = getaddrinfo(ip, port, &hints, &result);
		if (iResult == Result::Failure)
		{
			CHESSTER_ERROR("getaddrinfo failed with error: {0}", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			// Create a SOCKET for connecting to server
			m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (m_socket == INVALID_SOCKET) // Ensure that the socket is a valid socket.
			{
				CHESSTER_ERROR("socket failed with error: {0}", WSAGetLastError());
				freeaddrinfo(result);
				return false;
			}

			// Connect to server.
			iResult = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) // Check for general errors.
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		// Not needed anymore, free the memory
		freeaddrinfo(result);

		if (m_socket == INVALID_SOCKET)
		{
			CHESSTER_ERROR("Unable to connect to server!");
			return false;
		}

		return true;
	}

	bool ClientTCP::RecvBuffer()
	{
		// Prepare buffer
		char Buffer[1024]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		int iRecvResult = recv(m_BufferSocket, Buffer, BufferLen, 0);
		if (iRecvResult == SOCKET_ERROR)
		{
			CHESSTER_ERROR("RecvBuffer failed with error: {0}", WSAGetLastError());
			return false;
		}

		m_Data = std::string(Buffer);
		CHESSTER_INFO(m_Data);

		return true;
	}

	void ClientTCP::DNSLookup(const SOCKET& m_socket)
	{
		sockaddr_in ServerAddr{ NULL };
		int ServerAddrSize{ sizeof(ServerAddr) };
		char ServerName[NI_MAXHOST];
		char ServerPort[NI_MAXHOST];

		// Attempt to get the server's name
		int iResult = getpeername(m_socket, (sockaddr*)&ServerAddr, &ServerAddrSize);
		if (iResult == SOCKET_ERROR)
			CHESSTER_WARN("Unable to retrieve the server's address.");

		if (getnameinfo((sockaddr*)&ServerAddr, ServerAddrSize, ServerName, NI_MAXHOST, ServerPort, NI_MAXSERV, 0) == 0)
		{
			CHESSTER_INFO("Connected to {0} on port {1}", ServerName, ServerPort);
		}
		else // if unable to get name then show IP
		{
			inet_ntop(result->ai_family, &ServerAddr.sin_addr, ServerName, NI_MAXHOST);
			CHESSTER_INFO("Connected to {0} on port {1}", ServerName, ntohs(ServerAddr.sin_port));
		}
	}

	unsigned int __stdcall ClientTCP::ReceiveBuffer(void* data)
	{
		ClientTCP* clientTCP = static_cast<ClientTCP*>(data);

		while (true)
		{
			if (!clientTCP->RecvBuffer())
			{
				CHESSTER_ERROR("Failed to receive buffer. Disconnecting BufferSocket...");
				closesocket(m_BufferSocket);
				return Result::Failure;
			}
		}

		return Result::Success;
	}
}

#include "pch.h"
#include "Chesster_Unleashed/Connections/TCPConnection.h"

#include "Chesster_Unleashed/Layers/GameLayer.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	std::string TCPConnection::m_CameraData{ "" };
	std::string TCPConnection::m_RobotData{ "" };

	bool TCPConnection::CameraDataReceived{ false };
	bool TCPConnection::RobotDataReceived{ false };

	bool TCPConnection::IsCameraStreaming{ false };
	bool TCPConnection::IsServerListening{ false };

	TCPConnection* TCPConnection::s_Instance{ nullptr };

	TCPConnection::TCPConnection() :
		m_WSAData{ NULL },
		m_CameraCommandSocket{ INVALID_SOCKET },
		m_CameraBufferSocket{ INVALID_SOCKET },
		m_RobotListenSocket{ INVALID_SOCKET },
		m_RobotClientSocket{ INVALID_SOCKET }
	{
		s_Instance = this;

		// Initialize Winsock
		int iResult = WSAStartup(WINSOCK_VER, &m_WSAData);
		if (iResult != Result::Success) // Ensure system supports Winsock
		{
			LOG_ERROR("WSAStartup failed with error: {0}", iResult);
			throw std::runtime_error("Unable to initialize Winsock!");
		}
	}

	TCPConnection::~TCPConnection()
	{
		DisconnectCamera();
		DisconnectRobot();

		// Shut down the socket DLL
		WSACleanup();
	}

	void TCPConnection::ConnectCamera()
	{
		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		// For commanding the camera to take the picture
		std::string ip = { "localhost" }, port = { "23" };
		if (!CreateClientSocket(m_CameraCommandSocket, ip.c_str(), port.c_str()))
		{
			LOG_ERROR("Unable to connect m_CommandSocket. (IP: {0}, Port: {1})", ip, port);
			std::string str{ "Unable to connect m_CommandSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}
		else // If successful
		{
			DNSLookup(m_CameraCommandSocket);

			// Receive Cognex welcome message and username prompt
			recv(m_CameraCommandSocket, Buffer, BufferLen, 0);
			std::string str{ Buffer };
			str.erase(str.length() - sizeof("User: "), sizeof("User: ")); // Erase "User: "
			LOG_INFO(str);
			GameLayer::GetConsolePanel()->AddLog(str.c_str());

			// Send username
			std::string msg{ "admin\n" };
			send(m_CameraCommandSocket, msg.c_str(), msg.length(), 0);

			// Receive password prompt
			ZeroMemory(Buffer, BufferLen);
			recv(m_CameraCommandSocket, Buffer, BufferLen, 0);

			// Send password (no password, thus send \n)
			msg = { "\n" };
			send(m_CameraCommandSocket, msg.c_str(), msg.length(), 0);

			// Receive login confirmation
			ZeroMemory(Buffer, BufferLen);
			recv(m_CameraCommandSocket, Buffer, BufferLen, 0);
			str = { Buffer }; str.pop_back(); // pop '\n'
			LOG_INFO(str);

			str.insert(0, "\n"); str += "\n\n";
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}

		// For receiving data stream of physical board's status
		ip = { "localhost" }, port = { "3000" };
		if (!CreateClientSocket(m_CameraBufferSocket, ip.c_str(), port.c_str()))
		{
			LOG_ERROR("Unable to connect m_BufferSocket. (IP: {0}, Port: {1})", ip, port);
			std::string str{ "Unable to connect m_BufferSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}
		else // If successful
		{
			DNSLookup(m_CameraBufferSocket);

			// Create new thread for receiving data
			unsigned threadID{};
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &TCPConnection::CameraDataStreamThread, (void*)this, 0, &threadID);
		}

		Sleep(150);
	}

	void TCPConnection::DisconnectCamera()
	{
		IsCameraStreaming = false;

		closesocket(m_CameraCommandSocket);
		closesocket(m_CameraBufferSocket);
	}

	unsigned int __stdcall TCPConnection::ConnectRobotThread(void* data)
	{
		TCPConnection* TCP = static_cast<TCPConnection*>(data);

		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		std::string ip = { "192.168.7.10" }, port = { "15000" };
		if (!TCP->CreateServerSocket(TCP->m_RobotListenSocket, ip.c_str(), port.c_str()))
		{
			std::string str{ "Unable to connect m_RobotListenSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
			LOG_ERROR(str);
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
			SettingsPanel::IsRobotConnected = false;
			return false;
		}
		else
		{
			std::string str{ "Chesster server is ready." };
			LOG_INFO(str);
			GameLayer::GetConsolePanel()->AddLog(str.c_str());

			// Accept a client socket
			TCP->m_RobotClientSocket = accept(TCP->m_RobotListenSocket, (sockaddr*)&TCP->m_SockAddr, &TCP->m_SockAddrSize);
			if (TCP->m_RobotClientSocket == INVALID_SOCKET)
			{
				LOG_ERROR("WINSOCK: accept() failed with code: ", WSAGetLastError());
				closesocket(TCP->m_RobotClientSocket);
				SettingsPanel::IsRobotConnected = false;
				return false;
			}

			LOG_INFO("CS8C Connected.");

			IsServerListening = true;
			while (IsServerListening)
			{
				if (!TCP->RecvFromRobot())
				{
					closesocket(TCP->m_RobotClientSocket);
					SettingsPanel::IsRobotConnected = false;
					break;
				}
			}
		}

		return 0;
	}

	void TCPConnection::DisconnectRobot()
	{
		IsServerListening = false;

		closesocket(m_RobotListenSocket);
		closesocket(m_RobotClientSocket);

		std::string msg{ "Chesster server shut down." };
		LOG_INFO(msg);
		GameLayer::GetConsolePanel()->AddLog(msg.c_str());
	}

	bool TCPConnection::SendCameraCommand(const std::string& command)
	{
		int iSendResult = send(m_CameraCommandSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			LOG_ERROR("SendCameraCommand failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool TCPConnection::RecvCameraConfirmation()
	{
		// Prepare buffer
		char Buffer[8]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		// Receive confirmation from the camera
		int iRecvResult = recv(m_CameraCommandSocket, Buffer, BufferLen, 0);
		if (iRecvResult == INVALID_SOCKET)
		{
			LOG_ERROR("RecvConfirmation failed with error: {0}", WSAGetLastError());
			return false;
		}
		if (Buffer[0] != '1')
			return false;

		return true;
	}

	bool TCPConnection::SendToRobot(const std::string& command)
	{
		int iSendResult = send(m_RobotClientSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			LOG_ERROR("SendToRobot failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool TCPConnection::RecvFromRobot()
	{
		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		LOG_INFO("Waiting to receive...");
		int iResult = recv(m_RobotClientSocket, Buffer, BufferLen, 0);
		if (iResult > 0)
		{
			m_RobotData = Buffer;
			LOG_INFO(Buffer);
			GameLayer::GetConsolePanel()->AddLog(Buffer);
			RobotDataReceived = true;
		}

		Sleep(1000);
		return true;
	}

	bool TCPConnection::CreateClientSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
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
			LOG_ERROR("WINSOCK: getaddrinfo failed with error: {0}", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			// Create a SOCKET for connecting to server
			m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (m_socket == INVALID_SOCKET) // Ensure that the socket is a valid socket.
			{
				LOG_ERROR("WINSOCK: socket failed with error: {0}", WSAGetLastError());
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
			return false;

		return true;
	}

	bool TCPConnection::CreateServerSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
	{
		// Prepare addrinfo
		ZeroMemory(&hints, sizeof(hints));	// ZeroMemory fills the hints with zeros, prepares the memory to be used
		hints.ai_family = AF_INET;			// AF_NET for IPv4. AF_NET6 for IPv6. AF_UNSPEC for either (might cause error).
		hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.
		hints.ai_flags = AI_PASSIVE;		// Indicates the caller intends to use the returned socket address structure
											// in a call to the bind function.
		// Resolve the local address and port to be used by the server
		// The getaddrinfo function is used to determine the values in the sockaddr structure
		int iResult = getaddrinfo(ip, port, &hints, &result);
		if (iResult != Result::Success) // Error checking: ensure an address was received
		{
			LOG_ERROR("WINSOCK: getaddrinfo() failed with error: {0}", iResult);
			return false;
		}

		// Create a SOCKET for the server to listen for client connections
		m_RobotListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_RobotListenSocket == INVALID_SOCKET) // Error checking: ensure the socket is valid.
		{
			LOG_ERROR("WINSOCK: socket() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(result);
			return false;
		}

		// For a server to accept client connections,
		// it must be bound to a network address within the system.

		// Setup the TCP listening socket
		iResult = bind(m_RobotListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) // Error checking
		{
			LOG_ERROR("WINSOCK: bind() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(m_RobotListenSocket);
			return false;
		}

		// Once the bind function is called, the address information
		// returned by the getaddrinfo function is no longer needed
		freeaddrinfo(result);

		// After the socket is bound to an IP address and port on the system, the server
		// must then listen on that IP address and port for incoming connection requests.
		if (listen(m_RobotListenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			LOG_ERROR("WINSOCK: listen() failed with error: {0}", WSAGetLastError());
			closesocket(m_RobotListenSocket);
			return false;
		}

		return true;
	}

	void TCPConnection::DNSLookup(const SOCKET& m_socket)
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
				inet_ntop(result->ai_family, &m_SockAddr.sin_addr, ServerName, NI_MAXHOST);
				LOG_INFO("Connected to {0} on port {1}", ServerName, ntohs(m_SockAddr.sin_port));
			}
		}
	}

	bool TCPConnection::RecvCameraData()
	{
		// Prepare buffer
		char Buffer[256]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		int iRecvResult = recv(m_CameraBufferSocket, Buffer, BufferLen, 0);
		if (iRecvResult == SOCKET_ERROR)
		{
			LOG_ERROR("RecvBuffer failed with error: {0}. Disconnecting...", WSAGetLastError());
			DisconnectCamera();
			
			LOG_INFO("Camera disconnected.");
			GameLayer::GetConsolePanel()->AddLog("Camera disconnected.\n\n");
			return false;
		}

		m_CameraData = std::string(Buffer);
		CameraDataReceived = true;

		Sleep(1000);
		return true;
	}	

	unsigned int __stdcall TCPConnection::CameraDataStreamThread(void* data)
	{
		TCPConnection* clientTCP = static_cast<TCPConnection*>(data);

		IsCameraStreaming = true;
		while (IsCameraStreaming)
		{
			if (!clientTCP->RecvCameraData())
				return Result::Failure;
		}

		return Result::Success;
	}
}

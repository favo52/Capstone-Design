#include "pch.h"
#include "Chesster_Unleashed/Connections/ClientTCP.h"

#include "Chesster_Unleashed/Layers/GameLayer.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	std::string ClientTCP::m_CameraData{ "" };
	std::string ClientTCP::m_RobotData{ "" };
	SOCKET ClientTCP::m_CameraBufferSocket{ INVALID_SOCKET };
	//SOCKET ClientTCP::m_RobotLogSocket{ INVALID_SOCKET };

	bool ClientTCP::CameraDataReceived{ false };
	bool ClientTCP::RobotDataReceived{ false };

	bool ClientTCP::IsListening{ false };

	ClientTCP* ClientTCP::s_Instance{ nullptr };

	ClientTCP::ClientTCP() :
		m_WSAData{ NULL },
		m_CameraCommandSocket{ INVALID_SOCKET },
		m_RobotListenSocket{ INVALID_SOCKET }
		//m_RobotCommandSocket{ INVALID_SOCKET }
	{
		s_Instance = this;

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
		DisconnectCamera();
		DisconnectRobot();

		// Shut down the socket DLL
		WSACleanup();
	}

	void ClientTCP::ConnectCamera()
	{
		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		// For commanding the camera to take the picture
		std::string ip = { "localhost" }, port = { "23" };
		if (!CreateClientSocket(m_CameraCommandSocket, ip.c_str(), port.c_str()))
		{
			CHESSTER_ERROR("Unable to connect m_CommandSocket. (IP: {0}, Port: {1})", ip, port);
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
			CHESSTER_INFO(str);
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
			CHESSTER_INFO(str);

			str.insert(0, "\n"); str += "\n\n";
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}

		// For receiving data stream of physical board's status
		ip = { "localhost" }, port = { "3000" };
		if (!CreateClientSocket(m_CameraBufferSocket, ip.c_str(), port.c_str()))
		{
			CHESSTER_ERROR("Unable to connect m_BufferSocket. (IP: {0}, Port: {1})", ip, port);
			std::string str{ "Unable to connect m_BufferSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}
		else // If successful
		{
			DNSLookup(m_CameraBufferSocket);

			// Create new thread for receiving data
			unsigned threadID{};
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientTCP::CameraDataStream, (void*)&*this, 0, &threadID);
		}

		Sleep(150);
	}

	void ClientTCP::DisconnectCamera()
	{
		closesocket(m_CameraCommandSocket);
		closesocket(m_CameraBufferSocket);
	}

	//bool ClientTCP::ConnectRobot()
	//{
	//	// Prepare buffer
	//	char Buffer[128]{};
	//	int BufferLen{ sizeof(Buffer) };
	//	ZeroMemory(Buffer, BufferLen);
	//	
	//	//// Robot command socket
	//	//std::string ip = { "192.168.7.61" }, port = { "5653" };
	//	//if (!CreateClientSocket(m_RobotCommandSocket, ip.c_str(), port.c_str()))
	//	//{
	//	//	CHESSTER_ERROR("Unable to connect m_RobotCommandSocket. (IP: {0}, Port: {1})", ip, port);
	//	//	std::string str{ "Unable to connect m_RobotCommandSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
	//	//	GameLayer::GetConsolePanel()->AddLog(str.c_str());
	//	//	return false;
	//	//}
	//	//else
	//	//{
	//	//	DNSLookup(m_RobotCommandSocket);

	//	//	// Code here
	//	//	
	//	//	// Examples
	//	//	//std::string myMessage = { "Hello Robot" };
	//	//	//send(m_RobotCommandSocket, myMessage.c_str(), myMessage.length(), 0);
	//	//	//recv(m_RobotCommandSocket, Buffer, BufferLen, 0);
	//	//	//CHESSTER_INFO(Buffer); // print
	//	//}

	//	// Robot log socket
	//	//ip = { "" }, port = { "" }; // Need to figure out IP and PORT
	//	//if (!ConnectSocket(m_RobotLogSocket, ip.c_str(), port.c_str()))
	//	//{
	//	//	CHESSTER_ERROR("Unable to connect m_RobotLogSocket. (IP: {0}, Port: {1})", ip, port);
	//	//	std::string str{ "Unable to connect m_RobotLogSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
	//	//	GameLayer::GetConsolePanel()->AddLog(str.c_str());
	//	//}
	//	//else // If successful
	//	//{
	//	//	DNSLookup(m_RobotLogSocket);
	//	//
	//	//	// Create new thread for receiving data
	//	//	//unsigned threadID{};
	//	//	//HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientTCP::RobotDataStream, (void*)&*this, 0, &threadID);
	//	//}

	//	Sleep(150);
	//	return true;
	//}

	void ClientTCP::DisconnectRobot()
	{
		IsListening = false;

		//closesocket(m_RobotCommandSocket);
		//closesocket(m_RobotLogSocket);

		closesocket(m_RobotListenSocket);

		std::string msg{ "Chesster server shut down" };
		CHESSTER_INFO(msg);
		GameLayer::GetConsolePanel()->AddLog(msg.c_str());
	}

	bool ClientTCP::SendCameraCommand(const std::string& command)
	{
		int iSendResult = send(m_CameraCommandSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			CHESSTER_ERROR("SendCameraCommand failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool ClientTCP::RecvCameraConfirmation()
	{
		// Prepare buffer
		char Buffer[8]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		// Receive confirmation from the camera
		int iRecvResult = recv(m_CameraCommandSocket, Buffer, BufferLen, 0);
		if (iRecvResult == INVALID_SOCKET)
		{
			CHESSTER_ERROR("RecvConfirmation failed with error: {0}", WSAGetLastError());
			return false;
		}
		if (Buffer[0] != '1')
			return false;

		return true;
	}

	bool ClientTCP::SendToRobot(const std::string& command)
	{
		int iSendResult = send(m_RobotClientSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			CHESSTER_ERROR("SendToRobot failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool ClientTCP::RecvFromRobot()
	{
		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		//CHESSTER_INFO("Waiting to receive...");
		int iResult = recv(m_RobotClientSocket, Buffer, BufferLen, 0);
		if (iResult > 0)
		{
			CHESSTER_INFO(Buffer);
			GameLayer::GetConsolePanel()->AddLog(Buffer);
		}

		return true;
	}

	bool ClientTCP::CreateClientSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
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
			CHESSTER_ERROR("WINSOCK: getaddrinfo failed with error: {0}", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			// Create a SOCKET for connecting to server
			m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (m_socket == INVALID_SOCKET) // Ensure that the socket is a valid socket.
			{
				CHESSTER_ERROR("WINSOCK: socket failed with error: {0}", WSAGetLastError());
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

	bool ClientTCP::CreateServerSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port)
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
			CHESSTER_ERROR("WINSOCK: getaddrinfo() failed with error: {0}", iResult);
			return false;
		}

		// Create a SOCKET for the server to listen for client connections
		m_RobotListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (m_RobotListenSocket == INVALID_SOCKET) // Error checking: ensure the socket is valid.
		{
			CHESSTER_ERROR("WINSOCK: socket() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(result);
			return false;
		}

		// For a server to accept client connections,
		// it must be bound to a network address within the system.

		// Setup the TCP listening socket
		iResult = bind(m_RobotListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) // Error checking
		{
			CHESSTER_ERROR("WINSOCK: bind() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(result); // Called to free the memory allocated by the getaddrinfo function for this address information
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
			CHESSTER_ERROR("WINSOCK: listen() failed with error: {0}", WSAGetLastError());
			closesocket(m_RobotListenSocket);
			return false;
		}

		return true;
	}

	void ClientTCP::DNSLookup(const SOCKET& m_socket)
	{
		//sockaddr_in ServerAddr{ NULL };
		//int ServerAddrSize{ sizeof(ServerAddr) };
		char ServerName[NI_MAXHOST];
		char ServerPort[NI_MAXHOST];

		// Attempt to get the server's name
		int iResult = getpeername(m_socket, (sockaddr*)&m_SockAddr, &m_SockAddrSize);
		if (iResult == SOCKET_ERROR)
		{
			CHESSTER_WARN("Unable to retrieve the server's address.");
		}
		else
		{
			if (getnameinfo((sockaddr*)&m_SockAddr, m_SockAddrSize, ServerName, NI_MAXHOST, ServerPort, NI_MAXSERV, 0) == 0)
			{
				CHESSTER_INFO("Connected to {0} on port {1}", ServerName, ServerPort);
			}
			else // if unable to get name then show IP
			{
				inet_ntop(result->ai_family, &m_SockAddr.sin_addr, ServerName, NI_MAXHOST);
				CHESSTER_INFO("Connected to {0} on port {1}", ServerName, ntohs(m_SockAddr.sin_port));
			}
		}
	}

	bool ClientTCP::RecvCameraData()
	{
		// Prepare buffer
		char Buffer[256]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		int iRecvResult = recv(m_CameraBufferSocket, Buffer, BufferLen, 0);
		if (iRecvResult == SOCKET_ERROR)
		{
			CHESSTER_ERROR("RecvBuffer failed with error: {0}. Disconnecting...", WSAGetLastError());
			DisconnectCamera();
			CHESSTER_INFO("Camera disconnected.");
			GameLayer::GetConsolePanel()->AddLog("Camera disconnected.\n\n");
			return false;
		}

		m_CameraData = std::string(Buffer);
		CameraDataReceived = true;

		Sleep(1000);
		return true;
	}

	//bool ClientTCP::RecvRobotData()
	//{
	//	// Prepare buffer
	//	char Buffer[256]{};
	//	int BufferLen{ sizeof(Buffer) };
	//	ZeroMemory(Buffer, BufferLen);

	//	int iRecvResult = recv(m_RobotLogSocket, Buffer, BufferLen, 0);
	//	if (iRecvResult == SOCKET_ERROR)
	//	{
	//		CHESSTER_ERROR("RecvBuffer failed with error: {0}. Disconnecting...", WSAGetLastError());
	//		DisconnectCamera();
	//		CHESSTER_INFO("Robot disconnected.");
	//		GameLayer::GetConsolePanel()->AddLog("Robot disconnected.\n\n");
	//		return false;
	//	}

	//	//m_RobotData = std::string(Buffer);
	//	RobotDataReceived = true;

	//	Sleep(1000);
	//	return true;
	//}

	unsigned int __stdcall ClientTCP::ConnectRobot(void* data)
	{
		ClientTCP* TCP = static_cast<ClientTCP*>(data);

		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		std::string ip = { "192.168.7.10" }, port = { "15000" };
		if (!TCP->CreateServerSocket(TCP->m_RobotListenSocket, ip.c_str(), port.c_str()))
		{
			std::string str{ "Unable to connect m_RobotListenSocket. (IP: " + ip + ", Port: " + port + ")\n\n" };
			CHESSTER_ERROR(str);
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
			SettingsPanel::IsRobotConnected = false;
			return false;
		}
		else
		{
			CHESSTER_INFO("Chesster server is ready.");
			std::string str{ "Chesster server is ready." };
			GameLayer::GetConsolePanel()->AddLog(str.c_str());

			// Accept a client socket
			TCP->m_RobotClientSocket = accept(TCP->m_RobotListenSocket, (sockaddr*)&TCP->m_SockAddr, &TCP->m_SockAddrSize);
			if (TCP->m_RobotClientSocket == INVALID_SOCKET)
			{
				CHESSTER_ERROR("WINSOCK: accept() failed with code: ", WSAGetLastError());
				closesocket(TCP->m_RobotClientSocket);
				SettingsPanel::IsRobotConnected = false;
				return false;
			}

			// DNS Lookup
			//TCP->DNSLookup(TCP->m_RobotClientSocket);

			CHESSTER_INFO("CS8C Connected.");

			IsListening = true;
			while (IsListening)
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

	unsigned int __stdcall ClientTCP::CameraDataStream(void* data)
	{
		ClientTCP* clientTCP = static_cast<ClientTCP*>(data);

		while (true)
		{
			if (!clientTCP->RecvCameraData())
				return Result::Failure;
		}

		return Result::Success;
	}

	/*unsigned int __stdcall ClientTCP::RobotDataStream(void* data)
	{
		ClientTCP* clientTCP = static_cast<ClientTCP*>(data);

		while (true)
		{
			if (!clientTCP->RecvRobotData())
				return Result::Failure;
		}

		return Result::Success;
	}*/
}

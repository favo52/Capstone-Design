#include "pch.h"
#include "Chesster/Connections/TCPConnection.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	std::string TCPConnection::s_CameraData{ "" };
	std::string TCPConnection::s_RobotData{ "" };

	bool TCPConnection::CameraDataReceived{ false };
	bool TCPConnection::RobotDataReceived{ false };

	bool TCPConnection::IsCameraStreaming{ false };
	bool TCPConnection::IsServerListening{ false };

	std::string TCPConnection::s_CameraIP{ "localhost" };
	std::string TCPConnection::s_CameraCommandPort{ "23" };
	std::string TCPConnection::s_CameraStreamPort{ "3000" };

	std::string TCPConnection::s_RobotIP{ "192.168.7.10" };
	std::string TCPConnection::s_RobotPort{ "15000" };

	TCPConnection* TCPConnection::s_Instance{ nullptr };

	TCPConnection::TCPConnection() :
		m_CameraCommandSocket{ INVALID_SOCKET },
		m_CameraBufferSocket{ INVALID_SOCKET },
		m_ChessterListenSocket{ INVALID_SOCKET },
		m_RobotClientSocket{ INVALID_SOCKET }
	{
		s_Instance = this;
	}

	TCPConnection::~TCPConnection()
	{
		DisconnectCamera();
		DisconnectRobot();
	}

	void TCPConnection::ConnectCamera()
	{
		// For commanding the camera to take the picture
		if (!m_Winsock.CreateClientSocket(m_CameraCommandSocket, s_CameraIP.c_str(), s_CameraCommandPort.c_str()))
		{
			LOG_ERROR("Unable to connect m_CommandSocket. (IP: {0}, Port: {1})", s_CameraIP, s_CameraCommandPort);
			std::string str{ "Unable to connect m_CommandSocket. (IP: " + s_CameraIP + ", Port: " + s_CameraCommandPort + ")\n\n" };
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}
		else // If successful
		{
			m_Winsock.DNSLookup(m_CameraCommandSocket);

			// Prepare buffer
			char Buffer[128]{};
			int BufferLen{ sizeof(Buffer) };
			ZeroMemory(Buffer, BufferLen);

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
		if (!m_Winsock.CreateClientSocket(m_CameraBufferSocket, s_CameraIP.c_str(), s_CameraStreamPort.c_str()))
		{
			LOG_ERROR("Unable to connect m_BufferSocket. (IP: {0}, Port: {1})", s_CameraIP, s_CameraStreamPort);
			std::string str{ "Unable to connect m_BufferSocket. (IP: " + s_CameraIP + ", Port: " + s_CameraStreamPort + ")\n\n" };
			GameLayer::GetConsolePanel()->AddLog(str.c_str());
		}
		else // If successful
		{
			m_Winsock.DNSLookup(m_CameraBufferSocket);

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

		if (!TCP->m_Winsock.CreateServerSocket(TCP->m_ChessterListenSocket, s_RobotIP.c_str(), s_RobotPort.c_str()))
		{
			const std::string str{ "Unable create server socket. (IP: " + s_RobotIP + ", Port: " + s_RobotPort + ")\n\n" };
			LOG_ERROR(str);
			GameLayer::GetConsolePanel()->AddLog(str.c_str());

			SettingsPanel::IsRobotConnected = false;
			return 1;
		}

		const std::string str{ "Chesster server is ready." };
		LOG_INFO(str);
		GameLayer::GetConsolePanel()->AddLog(str.c_str());

		// Accept a client socket
		TCP->m_RobotClientSocket = TCP->m_Winsock.AcceptClient(TCP->m_ChessterListenSocket);
		if (TCP->m_RobotClientSocket == INVALID_SOCKET)
		{
			LOG_ERROR("WINSOCK: accept() failed with code: ", WSAGetLastError());
			closesocket(TCP->m_RobotClientSocket);
			SettingsPanel::IsRobotConnected = false;
			return 1;
		}

		LOG_INFO("CS8C Connected.");
		GameLayer::GetConsolePanel()->AddLog("CS8C Connected.");

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

		return 0;
	}

	void TCPConnection::DisconnectRobot()
	{
		IsServerListening = false;

		closesocket(m_ChessterListenSocket);
		closesocket(m_RobotClientSocket);

		const std::string msg{ "Chesster server shut down." };
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
		GameLayer::GetConsolePanel()->AddLog("Waiting to receive...");
		int iResult = recv(m_RobotClientSocket, Buffer, BufferLen, 0);
		if (iResult > 0)
		{
			s_RobotData = Buffer;
			LOG_INFO(Buffer);
			GameLayer::GetConsolePanel()->AddLog(Buffer);
			RobotDataReceived = true;
		}

		Sleep(1000);
		return true;
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

		s_CameraData = std::string(Buffer);
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
				return 1;
		}

		return 0;
	}
}

#include "pch.h"
#include "Chesster/Connections/Network.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	std::string Network::s_CameraData{ "" };
	std::string Network::s_RobotData{ "" };

	bool Network::CameraDataReceived{ false };
	bool Network::RobotDataReceived{ false };

	bool Network::IsCameraStreaming{ false };
	bool Network::IsServerListening{ false };

	Network* Network::s_Instance{ nullptr };

	Network::Network() :
		m_CameraCommandSocket{ INVALID_SOCKET },
		m_CameraBufferSocket{ INVALID_SOCKET },
		m_ChessterListenSocket{ INVALID_SOCKET },
		m_RobotClientSocket{ INVALID_SOCKET }
	{
		s_Instance = this;
	}

	Network::~Network()
	{
		DisconnectCamera();
		DisconnectRobot();
	}

	unsigned int __stdcall Network::ConnectCameraThread(void* data)
	{
		Network* TCP = static_cast<Network*>(data);
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();
		SettingsPanel* settingsPanel = GameLayer::Get().GetSettingsPanel();

		// For commanding the camera to take the picture
		const std::string& cameraIP = settingsPanel->GetCameraIP();
		const std::string& cameraCommandPort = settingsPanel->GetCamCommandPort();
		if (!TCP->m_Winsock.CreateClientSocket(TCP->m_CameraCommandSocket, cameraIP, cameraCommandPort))
		{
			std::string str{ "Unable to connect the Camera Command Socket. "
				"(IP: " + cameraIP + ", Port: " + cameraCommandPort + ")\n" };
			LOG_ERROR(str);
			consolePanel->AddLog(str);
			settingsPanel->SetCameraButtonStatus(false);
			return 1;
		}

		// Attempt to login to the cognex camera
		if (!TCP->LoginToCognex())
		{
			std::string str{ "Unable to login to the Cognex camera.\n" };
			LOG_ERROR(str);
			consolePanel->AddLog(str);
			settingsPanel->SetCameraButtonStatus(false);
			return 1;
		}

		// For receiving data stream of physical board's status
		const std::string& cameraStreamPort = settingsPanel->GetCamStreamPort();
		if (!TCP->m_Winsock.CreateClientSocket(TCP->m_CameraBufferSocket, cameraIP, cameraStreamPort))
		{
			std::string str{ "Unable to connect the Camera Buffer Socket. "
				"(IP: " + cameraIP + ", Port: " + cameraStreamPort + ")\n" };
			LOG_ERROR(str);
			consolePanel->AddLog(str);
			settingsPanel->SetCameraButtonStatus(false);
			return 1;
		}
		
		IsCameraStreaming = true;
		while (IsCameraStreaming)
		{
			if (!TCP->RecvCameraData())
			{
				std::string str{ "Stopped receiving camera data." };
				LOG_ERROR(str);
				consolePanel->AddLog(str);
				return 1;
			}
		}

		return 0;
	}

	unsigned int __stdcall Network::ConnectRobotThread(void* data)
	{
		Network* TCP = static_cast<Network*>(data);

		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();
		SettingsPanel* settingsPanel = GameLayer::Get().GetSettingsPanel();
		
		const std::string& robotIP = settingsPanel->GetRobotIP();
		const std::string& robotPort = settingsPanel->GetRobotPort();
		if (!TCP->m_Winsock.CreateServerSocket(TCP->m_ChessterListenSocket, robotIP, robotPort))
		{
			const std::string str{ "Unable create server socket. (IP: " + robotIP + ", Port: " + robotPort + ")\n\n" };
			LOG_ERROR(str);
			consolePanel->AddLog(str);
			settingsPanel->SetRobotButtonStatus(false);
			return 1;
		}

		const std::string str{ "Chesster server is ready." };
		LOG_INFO(str);
		consolePanel->AddLog(str);

		// Accept a client socket
		TCP->m_RobotClientSocket = TCP->m_Winsock.AcceptClient(TCP->m_ChessterListenSocket);
		if (TCP->m_RobotClientSocket == INVALID_SOCKET)
		{
			LOG_ERROR("WINSOCK: accept() failed with code: ", WSAGetLastError());
			closesocket(TCP->m_ChessterListenSocket);
			settingsPanel->SetRobotButtonStatus(false);
			return 1;
		}

		// No longer need server listening socket
		closesocket(TCP->m_ChessterListenSocket);

		LOG_INFO("CS8C Connected.");
		consolePanel->AddLog("CS8C Connected.");

		IsServerListening = true;
		while (IsServerListening)
		{
			if (!TCP->RecvFromRobot())
			{
				closesocket(TCP->m_RobotClientSocket);
				settingsPanel->SetRobotButtonStatus(false);
				IsServerListening = false;
				break;
			}
		}

		return 0;
	}

	void Network::DisconnectCamera()
	{
		IsCameraStreaming = false;

		closesocket(m_CameraCommandSocket);
		closesocket(m_CameraBufferSocket);
	}

	void Network::DisconnectRobot()
	{
		IsServerListening = false;

		closesocket(m_ChessterListenSocket);
		closesocket(m_RobotClientSocket);
	}

	bool Network::SendToCamera(const std::string& command)
	{
		int iSendResult = send(m_CameraCommandSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			LOG_ERROR("SendToCamera failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool Network::RecvFromCamera(std::array<char, 128>& buffer)
	{
		// Empty the buffer
		buffer = {};

		// Receive confirmation from the camera
		int iRecvResult = recv(m_CameraCommandSocket, buffer.data(), buffer.size(), 0);
		if (iRecvResult == INVALID_SOCKET)
		{
			LOG_ERROR("RecvFromCamera failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool Network::SendToRobot(const std::string& command)
	{
		int iSendResult = send(m_RobotClientSocket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			LOG_ERROR("SendToRobot failed with error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool Network::RecvFromRobot()
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		// Prepare buffer
		char Buffer[128]{};
		int BufferLen{ sizeof(Buffer) };
		ZeroMemory(Buffer, BufferLen);

		LOG_INFO("Waiting to receive...");
		consolePanel->AddLog("Waiting to receive...");
		int iResult = recv(m_RobotClientSocket, Buffer, BufferLen, 0);
		if (iResult > 0)
		{
			s_RobotData = Buffer;
			LOG_INFO(Buffer);
			consolePanel->AddLog(Buffer);
			RobotDataReceived = true;
		}

		Sleep(1000);
		return true;
	}

	bool Network::LoginToCognex()
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();
		std::array<char, 128> buffer = {};

		// Receive Cognex welcome message
		if (!RecvFromCamera(buffer))
			return false;
		LOG_INFO(buffer.data());
		consolePanel->AddLog(buffer.data());

		// Receive User name prompt
		if (!RecvFromCamera(buffer))
			return false;
		LOG_INFO("{0} {1}", buffer.data(), "admin");
		consolePanel->AddLog(buffer.data() + std::string("admin"));

		// Send User name
		if (!SendToCamera("admin\n"))
			return false;

		// Receive Password prompt
		if (!RecvFromCamera(buffer))
			return false;
		LOG_INFO(buffer.data());
		consolePanel->AddLog(buffer.data());

		// Send password (no password, thus we "press enter")
		if (!SendToCamera("\n"))
			return false;

		// Receive login confirmation
		if (!RecvFromCamera(buffer))
			return false;
		LOG_INFO(buffer.data());
		consolePanel->AddLog(buffer.data());

		// Send take initial picture command
		if (!SendToCamera("SE8\n"))
			return false;

		// Receive picture confirmation
		if (!RecvFromCamera(buffer))
			return false;

		if (buffer.size() > 0 && buffer.front() != '1')
		{
			LOG_WARN("Camera did not take picture.");
			GameLayer::Get().GetConsolePanel()->AddLog("Camera did not take picture.");
		}

		return true;
	}

	bool Network::RecvCameraData()
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

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
			consolePanel->AddLog("Camera disconnected.\n\n");
			return false;
		}

		s_CameraData = std::string(Buffer);
		CameraDataReceived = true;

		Sleep(1000);
		return true;
	}
}

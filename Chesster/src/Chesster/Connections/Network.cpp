#include "pch.h"
#include "Chesster/Connections/Network.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	std::array<char, 512> Network::m_CameraDataBuffer = {};

	Network::Network() :
		m_CameraTelnetSocket{ INVALID_SOCKET },
		m_CameraTCPDeviceSocket{ INVALID_SOCKET },
		m_ChessterListenSocket{ INVALID_SOCKET },
		m_RobotClientSocket{ INVALID_SOCKET }
	{
	}

	Network::~Network()
	{
		DisconnectCamera();
		DisconnectRobot();
	}

	void Network::CameraTelnetThread()
	{
		Network& network = GameLayer::Get().GetNetwork();
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();
		SettingsPanel& settingsPanel = GameLayer::Get().GetSettingsPanel();

		// For commanding the camera to take the picture
		const std::string& cameraIP = settingsPanel.m_CameraIP;
		//const std::string& cameraIP = "localhost";
		const std::string& cameraTelnetPort = settingsPanel.m_CameraTelnetPort;
		if (!network.CreateClientSocket(network.m_CameraTelnetSocket, cameraIP, cameraTelnetPort))
		{
			const std::string str{ "Unable to connect the Camera Command Socket. "
				"(IP: " + cameraIP + ", Port: " + cameraTelnetPort + ")" };
			LOG_ERROR(str);
			consolePanel.AddLog(str);
			network.DisconnectCamera();
			settingsPanel.SetCameraButtonStatus(false);
			return;
		}
		LOG_INFO("Telnet Connected. (ip: {0}, port: {1})", cameraIP, cameraTelnetPort);

		network.SendToCamera(network.m_CameraTelnetSocket, "admin\r\n");
		network.SendToCamera(network.m_CameraTelnetSocket, "\r\n");

		// Attempt to login to Cognex InSight-Explorer
		//std::array<char, 128> tempbuffer = {};
		//if (network.RecvCameraCommand(tempbuffer))
		{
			//network.SendToCamera(network.m_CameraTelnetSocket, "admin\r\n");
			//network.SendToCamera(network.m_CameraTelnetSocket, "\r\n");
			//LOG_INFO("Received1: {0}", tempbuffer.data()); // welcome
			//if (network.RecvCameraCommand(tempbuffer))
			{
				//LOG_INFO("Received2 {0}", tempbuffer.data()); // user
				
				//network.SendToCamera("admin\r\n \0");
				//if (!network.SendToCamera("admin\r\n\r\n"))
				//{
				//	const std::string str{ "Unable to login to the Cognex InSight-Explorer." };
				//	LOG_ERROR(str);
				//	consolePanel.AddLog(str);
				//	network.DisconnectCamera();
				//	settingsPanel.SetCameraButtonStatus(false);
				//	return;
				//}
			}
		}

		// Keep thread alive waiting for any new data received from
		// the InSight-Explorer Telnet socket
		while (true)
		{
			std::array<char, 128> buffer = {};
			if (!network.RecvCameraCommand(buffer))
				break;

			LOG_INFO("TelnetBuffer: {0}", buffer.data());			
			consolePanel.AddLog(buffer.data());
		}

		network.DisconnectCamera();
		settingsPanel.SetCameraButtonStatus(false);
	}

	void Network::CameraTCPDeviceThread()
	{
		Network& network = GameLayer::Get().GetNetwork();
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();
		SettingsPanel& settingsPanel = GameLayer::Get().GetSettingsPanel();

		// For receiving data stream of physical board's status
		const std::string& cameraIP = settingsPanel.m_CameraIP;
		const std::string& cameraTCPDevicePort = settingsPanel.m_CameraTCPDevicePort;
		if (!network.CreateClientSocket(network.m_CameraTCPDeviceSocket, cameraIP, cameraTCPDevicePort))
		{
			const std::string str{ "Unable to connect the Camera Buffer Socket. "
				"(IP: " + cameraIP + ", Port: " + cameraTCPDevicePort + ")" };
			LOG_ERROR(str);
			consolePanel.AddLog(str);
			network.DisconnectCamera();
			settingsPanel.SetCameraButtonStatus(false);
			return;
		}
		LOG_INFO("TCP Device Connected. (ip: {0}, port: {1})", cameraIP, cameraTCPDevicePort);

		network.SendToCamera(network.m_CameraTCPDeviceSocket, "admin\r\n");
		network.SendToCamera(network.m_CameraTCPDeviceSocket, "\r\n");
		network.SendToCamera(network.m_CameraTCPDeviceSocket, "IMG\r\n");
		//network.SendToCamera(network.m_CameraTCPDeviceSocket, "DAT\r\n");

		// Keep thread alive waiting for any new data received from
		// the InSight-Explorer TCP Device socket
		while (true)
		{
			if (!network.RecvCameraData(m_CameraDataBuffer))
			{
				const std::string str{ "Stopped receiving camera data." };
				LOG_INFO(str);
				consolePanel.AddLog(str);
				break;
			}

			LOG_INFO("TCP Device Buffer: {0}", m_CameraDataBuffer.data());
		}

		network.DisconnectCamera();
		settingsPanel.SetCameraButtonStatus(false);
	}

	void Network::ChessterRobotThread()
	{
		Network& network = GameLayer::Get().GetNetwork();
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();
		SettingsPanel& settingsPanel = GameLayer::Get().GetSettingsPanel();
		
		const std::string& robotIP = settingsPanel.m_RobotIP;
		const std::string& robotPort = settingsPanel.m_RobotPort;
		if (!network.CreateServerSocket(network.m_ChessterListenSocket, robotIP, robotPort))
		{
			const std::string str{ "Unable create server socket. (IP: " + robotIP + ", Port: " + robotPort + ")" };
			LOG_ERROR(str);
			consolePanel.AddLog(str);
			network.DisconnectRobot();
			settingsPanel.SetRobotButtonStatus(false);
			return;
		}

		const std::string str{ "Chesster server is ready. Accepting client..." };
		LOG_INFO(str);
		consolePanel.AddLog(str);

		// Accept a client (the staubli robot)
		network.m_RobotClientSocket = network.AcceptClient(network.m_ChessterListenSocket);
		if (network.m_RobotClientSocket == INVALID_SOCKET)
		{
			LOG_ERROR("WINSOCK: accept() failed with code: ", WSAGetLastError());
			network.DisconnectRobot();
			settingsPanel.SetRobotButtonStatus(false);
			return;
		}

		// No longer need server listening socket
		closesocket(network.m_ChessterListenSocket);

		LOG_INFO("Client accepted. CS8C Connected.");
		consolePanel.AddLog("Client accepted. CS8C Connected.");

		// Keep thread alive waiting for any new
		// data received from the robot's client socket
		while (true)
		{
			std::array<char, 256> buffer = {};
			if (!network.RecvFromRobot(buffer))
				break;
		}

		network.DisconnectRobot();
		settingsPanel.SetRobotButtonStatus(false);
	}

	void Network::DisconnectCamera()
	{
		closesocket(m_CameraTelnetSocket);
		m_CameraTelnetSocket = INVALID_SOCKET;

		closesocket(m_CameraTCPDeviceSocket);
		m_CameraTCPDeviceSocket = INVALID_SOCKET;
	}

	void Network::DisconnectRobot()
	{
		closesocket(m_ChessterListenSocket);
		m_ChessterListenSocket = INVALID_SOCKET;

		closesocket(m_RobotClientSocket);
		m_RobotClientSocket = INVALID_SOCKET;
	}

	bool Network::SendToCamera(SOCKET& socket, const std::string& command)
	{
		//int iSendResult = send(m_CameraTelnetSocket, command.c_str(), command.length(), 0);
		int iSendResult = send(socket, command.c_str(), command.length(), 0);
		if (iSendResult == INVALID_SOCKET)
		{
			LOG_ERROR("SendToCamera failed with error: {0}", WSAGetLastError());
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
	
	void Network::TakePicture()
	{
		SendToCamera(m_CameraTelnetSocket, "SE8\r\n");
	}

	bool Network::RecvCameraCommand(std::array<char, 128>& buffer)
	{
		buffer = {};
		int iRecvResult = recv(m_CameraTelnetSocket, buffer.data(), buffer.size(), 0);
		if (iRecvResult == INVALID_SOCKET)
		{
			LOG_ERROR("RecvCameraCommand failed with error: {0}. Disconnecting...", WSAGetLastError());
			DisconnectCamera();
			return false;
		}

		return true;
	}

	bool Network::RecvCameraData(std::array<char, 512>& buffer)
	{
		int iRecvResult = recv(m_CameraTCPDeviceSocket, buffer.data(), buffer.size(), 0);
		if (iRecvResult == SOCKET_ERROR)
		{
			LOG_ERROR("RecvCameraData failed with error: {0}. Disconnecting...", WSAGetLastError());
			DisconnectCamera();
			
			LOG_INFO("Camera buffer disconnected.");
			GameLayer::Get().GetConsolePanel().AddLog("Camera buffer disconnected.");
			return false;
		}
		
		if (iRecvResult > 0)
			GameLayer::Get().SetCameraDataReceived(true);

		return true;
	}

	bool Network::RecvFromRobot(std::array<char, 256>& buffer)
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();
		
		LOG_INFO("Waiting to receive...");
		consolePanel.AddLog("Waiting to receive...");

		int iRecvResult = recv(m_RobotClientSocket, buffer.data(), buffer.size(), 0);
		if (iRecvResult == INVALID_SOCKET)
		{
			LOG_ERROR("RecvFromRobot failed with error: {0}. Disconnecting...", WSAGetLastError());
			return false;
		}

		if (iRecvResult > 0)
		{
			LOG_INFO(buffer.data());
			consolePanel.AddLog(buffer.data());
		}

		return true;
	}
}

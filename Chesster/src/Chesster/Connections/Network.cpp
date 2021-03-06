/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "pch.h"
#include "Chesster/Connections/Network.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	std::array<char, 256> Network::m_CameraDataBuffer = {};

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
		LogPanel& logPanel = GameLayer::Get().GetLogPanel();
		SettingsPanel& settingsPanel = GameLayer::Get().GetSettingsPanel();

		// For commanding the camera to take the picture
		const std::string& cameraIP = settingsPanel.m_CameraIP;
		const std::string& cameraTelnetPort = settingsPanel.m_CameraTelnetPort;
		if (!network.CreateClientSocket(network.m_CameraTelnetSocket, cameraIP, cameraTelnetPort))
		{
			const std::string str{ "Unable to connect the Camera Telnet Socket. "
				"(IP: " + cameraIP + ", Port: " + cameraTelnetPort + ")" };
			LOG_ERROR(str);
			logPanel.AddLog(str);
			network.DisconnectCamera();
			settingsPanel.SetCameraButtonStatus(false);
			return;
		}
		LOG_INFO("Telnet Connected. (IP: {0}, Port: {1})", cameraIP, cameraTelnetPort);

		// Attempt to login to Cognex Camera
		network.SendToCamera("admin\r\n");	// User
		network.SendToCamera("\r\n");		// Password

		// Keep thread alive waiting for any new data
		// received from the InSight-Explorer Telnet socket
		while (network.m_CameraTelnetSocket)
		{
			std::array<char, 128> buffer = {};
			if (network.RecvData(network.m_CameraTelnetSocket, buffer))
			{
				if (buffer.front() == '1')
					logPanel.AddLog("Picture taken.");
				else
				{
					LOG_INFO("Telnet Buffer: {0}", buffer.data());
					logPanel.AddLog("Telnet Buffer: " + std::string(buffer.data()));
				}
			}
			else
			{
				const std::string str{ "CameraTelnetThread ended." };
				LOG_INFO(str);
				logPanel.AddLog(str);
				settingsPanel.SetCameraButtonStatus(false);
				break;
			}
		}
	}

	void Network::CameraTCPDeviceThread()
	{
		Network& network = GameLayer::Get().GetNetwork();
		LogPanel& logPanel = GameLayer::Get().GetLogPanel();
		SettingsPanel& settingsPanel = GameLayer::Get().GetSettingsPanel();

		// For receiving data stream of physical board's status
		const std::string& cameraIP = settingsPanel.m_CameraIP;
		const std::string& cameraTCPDevicePort = settingsPanel.m_CameraTCPDevicePort;
		if (!network.CreateClientSocket(network.m_CameraTCPDeviceSocket, cameraIP, cameraTCPDevicePort))
		{
			const std::string str{ "Unable to connect the TCP Device Socket. "
				"(IP: " + cameraIP + ", Port: " + cameraTCPDevicePort + ")" };
			LOG_ERROR(str);
			logPanel.AddLog(str);
			network.DisconnectCamera();
			settingsPanel.SetCameraButtonStatus(false);
			return;
		}
		LOG_INFO("TCP Device Connected. (IP: {0}, Port: {1})", cameraIP, cameraTCPDevicePort);

		// Keep thread alive waiting for any new data received
		// from the InSight-Explorer TCP Device socket
		while (network.m_CameraTCPDeviceSocket)
		{
			std::array<char, 256> buffer = {};
			if (network.RecvData(network.m_CameraTCPDeviceSocket, buffer))
			{
				m_CameraDataBuffer = buffer;
				GameLayer::Get().CameraDataReceived();
			}
			else
			{
				const std::string str{ "CameraTCPDeviceThread ended." };
				LOG_INFO(str);
				logPanel.AddLog(str);
				settingsPanel.SetCameraButtonStatus(false);
				break;
			}
		}
	}

	void Network::ChessterServerThread()
	{
		Network& network = GameLayer::Get().GetNetwork();
		LogPanel& logPanel = GameLayer::Get().GetLogPanel();
		SettingsPanel& settingsPanel = GameLayer::Get().GetSettingsPanel();
		
		const std::string& serverIP = settingsPanel.m_ServerIP;
		const std::string& serverPort = settingsPanel.m_ServerPort;
		if (!network.CreateServerSocket(network.m_ChessterListenSocket, serverIP, serverPort))
		{
			const std::string str{ "Unable create server socket. (IP: " + serverIP + ", Port: " + serverPort + ")" };
			LOG_ERROR(str);
			logPanel.AddLog(str);
			network.DisconnectRobot();
			settingsPanel.SetRobotButtonStatus(false);
			return;
		}

		const std::string str{ "Chesster server is ready. Accepting client..." };
		LOG_INFO(str);
		logPanel.AddLog(str);

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
		logPanel.AddLog("Client accepted. CS8C Connected.");

		// Keep thread alive waiting for any new
		// data received from the robot's client socket
		while (network.m_RobotClientSocket)
		{
			std::array<char, 8> buffer = {};
			if (network.RecvData(network.m_RobotClientSocket, buffer))
			{
				LOG_INFO("Received From Robot: {0}", buffer.data());

				if (buffer.size() > 2)
				{
					GameLayer& gameLayer = GameLayer::Get();

					const char NewGameButton = buffer[0];
					const char EndTurnButton = buffer[1];
					const char ArmSettled = buffer[2];
					
					if (NewGameButton == '1') // Green button pressed
					{
						gameLayer.NewGameButtonPressed();
						network.SendToCamera("SE8\r\n");
					}

					if (EndTurnButton == '1') // Black button is pressed
					{
						gameLayer.EndPlayerTurn();
						network.SendToCamera("SE8\r\n");
					}

					if (ArmSettled == '1') // ArmSettled
					{
						switch (gameLayer.GetGameState())
						{
							case GameLayer::GameState::Gameplay:
							{
								gameLayer.ArmIsSettled();
								network.SendToCamera("SE8\r\n");	// Take picture
								network.SendToRobot(GAME_ACTIVE);	// Keep playing
								break;
							}

							case GameLayer::GameState::Checkmate:
							{
								network.SendToRobot(BLACK_WON);		// Robot will celebrate it won
								break;
							}

							case GameLayer::GameState::Stalemate:
							{
								network.SendToRobot(STALEMATE);	// Don't do anything when Stalemate
								break;
							}
						}
					}
				}
			}
			else
			{
				LOG_INFO("ChessterServerThread ended.");
				break;
			}
		}

		network.DisconnectRobot();
		settingsPanel.SetRobotButtonStatus(false);
	}

	void Network::ShutdownCamera()
	{
		shutdown(m_CameraTelnetSocket, SD_BOTH);
		shutdown(m_CameraTCPDeviceSocket, SD_BOTH);
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

	bool Network::SendToCamera(const std::string& data)
	{
		int sendResult = send(m_CameraTelnetSocket, data.c_str(), data.length(), 0);
		if (sendResult == SOCKET_ERROR)
		{
			LOG_ERROR("SendToCamera socket error: {0}", WSAGetLastError());
			return false;
		}

		LOG_INFO("Sent To Camera: {0}", data);

		return true;
	}

	bool Network::SendToRobot(const std::string& data)
	{
		int sendResult = send(m_RobotClientSocket, data.c_str(), data.length(), 0);
		if (sendResult == SOCKET_ERROR)
		{
			LOG_ERROR("SendToRobot socket error: {0}", WSAGetLastError());
			return false;
		}

		LOG_INFO("Sent To Robot: {0}", data);

		return true;
	}

	template<size_t S>
	bool Network::RecvData(const SOCKET& socket, std::array<char, S>& buffer)
	{
		buffer = {};	// Empty the buffer
		int recvResult = recv(socket, buffer.data(), buffer.size(), 0);
		if (recvResult == SOCKET_ERROR)
		{
			LOG_ERROR("RecvData socket error: {0}. Disconnecting...", WSAGetLastError());
			return false;
		}

		if (recvResult > 0)
			return true;

		return false;
	}
}

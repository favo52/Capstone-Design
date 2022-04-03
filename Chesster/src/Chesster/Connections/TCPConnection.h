#pragma once

#include "Chesster/Connections/Winsock.h"

// Cognex camera login
// User: admin
// Pass: 

// Command to take pic
// SE8
// if received 1 success
// else error

namespace Chesster
{
	/*	 */
	class TCPConnection
	{
	public:
		TCPConnection();
		virtual ~TCPConnection();

		// Camera connection
		void ConnectCamera();
		void DisconnectCamera();

		// Robot connection
		static unsigned int __stdcall ConnectRobotThread(void* data);
		void DisconnectRobot();

		bool SendCameraCommand(const std::string& command);
		bool RecvCameraConfirmation();

		bool SendToRobot(const std::string& command);
		bool RecvFromRobot();

		const std::string& GetCameraData() const { return s_CameraData; }
		const std::string& GetRobotData() const { return s_RobotData; }

		static TCPConnection& Get() { return *s_Instance; }

	public:
		static bool CameraDataReceived;
		static bool RobotDataReceived;

		static bool IsCameraStreaming;
		static bool IsServerListening;

		static std::string s_CameraIP, s_CameraCommandPort, s_CameraStreamPort;
		static std::string s_RobotIP, s_RobotPort;

	private:
		bool RecvCameraData();
		static unsigned int __stdcall CameraDataStreamThread(void* data);

	private:
		Winsock m_Winsock;

		SOCKET m_CameraCommandSocket;	// Command the camera to take a pic
		SOCKET m_CameraBufferSocket;	// Receive the camera's data

		SOCKET m_ChessterListenSocket;	// SOCKET for server to listen for client connections
		SOCKET m_RobotClientSocket;		// SOCKET for accepting connection from staubli robot

	private:
		static std::string s_CameraData;
		static std::string s_RobotData;

		static TCPConnection* s_Instance; // Pointer to this
	};
}

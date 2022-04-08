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
	class Network
	{
	public:
		Network();
		virtual ~Network();

		static unsigned int __stdcall ConnectCameraThread(void* data);
		static unsigned int __stdcall ConnectRobotThread(void* data);

		void DisconnectCamera();
		void DisconnectRobot();

		bool SendCameraCommand(const std::string& command);
		bool RecvCameraConfirmation();

		bool SendToRobot(const std::string& command);
		bool RecvFromRobot();

		const std::string& GetCameraData() const { return s_CameraData; }
		const std::string& GetRobotData() const { return s_RobotData; }

		static Network& Get() { return *s_Instance; }

	public:
		static bool CameraDataReceived;
		static bool RobotDataReceived;

		static bool IsCameraStreaming;
		static bool IsServerListening;

	private:
		bool LoginToCognex();
		bool RecvCameraData();

	private:
		Winsock m_Winsock;				// Initializes Windows Sockets

		SOCKET m_CameraCommandSocket;	// Client SOCKET to give commands to the camera
		SOCKET m_CameraBufferSocket;	// Client SOCKET to receive the camera's data

		SOCKET m_ChessterListenSocket;	// Server SOCKET to listen for client connections
		SOCKET m_RobotClientSocket;		// Client SOCKET for accepting connection from staubli robot

	private:
		static std::string s_CameraData;
		static std::string s_RobotData;

		static Network* s_Instance;		// Pointer to this
	};
}

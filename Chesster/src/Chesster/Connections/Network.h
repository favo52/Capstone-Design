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

		static unsigned int __stdcall CameraTelnetThread(void* data);

		static unsigned int __stdcall CameraTCPDeviceThread(void* data);

		static unsigned int __stdcall ChessterRobotThread(void* data);

		void DisconnectCamera();
		
		void DisconnectRobot();

		bool SendToCamera(const std::string& command);

		bool SendToRobot(const std::string& command);

		/** Retrieves the instance of the current Network object.
		 @return A reference to this Network object. */
		static Network& Get() { return *s_Instance; }

	private:
		bool LoginToCognex();
		bool RecvCameraCommand(std::array<char, 128>& buffer);
		bool RecvCameraData(std::array<char, 256>& buffer);
		bool RecvFromRobot(std::array<char, 256>& buffer);

	private:
		Winsock m_Winsock;				// Initializes Windows Sockets

		SOCKET m_CameraTelnetSocket;	// Client SOCKET to give commands to the camera
		SOCKET m_CameraTCPDeviceSocket;	// Client SOCKET to receive the camera's data

		SOCKET m_ChessterListenSocket;	// Server SOCKET to listen for client connections
		SOCKET m_RobotClientSocket;		// Client SOCKET for accepting connection from staubli robot

	private:
		static Network* s_Instance;		// Pointer to this
	};
}

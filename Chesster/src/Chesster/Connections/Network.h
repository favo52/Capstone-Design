#pragma once

#include "Chesster/Connections/Winsock.h"

// Cognex camera login
// User: admin
// Pass: 

// Command to take pic: SE8
// if received 1 success
// else error

namespace Chesster
{
	/*	 */
	class Network : public Winsock
	{
	public:
		/*	Initializes all SOCKETs to INVALID_SOCKET and a pointer to itself. */
		Network();

		/*	Closes all SOCKETs if there's any left open. */
		virtual ~Network();

		/*	To be used when creating a new thread.
			Establishes a TCP client SOCKET connection between the localhost PC and
			the Cognex In-Sight Explorer software. It allows the user to send commands
			and receive corresponding messages. */
		static void CameraTelnetThread();

		/*	To be used when creating a new thread.
			Establishes a TCP client SOCKET connection between the localhost PC and
			the Cognex In-Sight Explorer software. It allows the CHESSTER program to receive
			the new board positions each time the Cognex camera takes a new picture. */
		static void CameraTCPDeviceThread();

		/*	To be used when creating a new thread.
			Enables the CHESSTER program to act as a TCP server. This allows the Staubli robot to
			connect as a client and establish TCP/IP communication. */
		static void ChessterRobotThread();

		/*	Closes the Camera Telnet and TCPDevice SOCKETs. */
		void DisconnectCamera();
		
		/*	Closes the Chesster listening SOCKET and the Robot Client SOCKET. */
		void DisconnectRobot();

		/**	Sends a message through TCP/IP to the camera.
		 @param command The message to be sent.
		 @return True if the message was sent succesfully, false otherwise. */
		bool SendToCamera(const std::string& command);

		/**	Sends a message through TCP/IP to the staubli robot.
		 @param command The message to be sent.
		 @return True if the message was sent succesfully, false otherwise. */
		bool SendToRobot(const std::string& command);

		/**	Retrieves the buffer that stores the data received from the Cognex Camera.
		 @return An char array of size 256 containing the piece location on the physical board. */
		static const std::array<char, 256>& GetCameraBuffer() { return m_CameraDataBuffer; }

	private:
		/**	Waits to receive any new data from the Camera Telnet SOCKET. 
			This function is used in its own thread so it doesn't lock up the program.
		 @param buffer The buffer where the received data will be stored.
		 @return True if a message was received successfully, false if the communication was closed. */
		bool RecvCameraCommand(std::array<char, 128>& buffer);

		/**	Waits to receive any new data from the Camera TCP Device SOCKET. 
			This function is used in its own thread so it doesn't lock up the program.
		 @param buffer The buffer where the received data will be stored.
		 @return True if a message was received successfully, false if the communication was closed. */
		bool RecvCameraData(std::array<char, 256>& buffer);

		/**	Waits to receive any new data from the Robot Client SOCKET.
			This function is used in its own thread so it doesn't lock up the program.
		 @param buffer The buffer where the received data will be stored.
		 @return True if a message was received successfully, false if the communication was closed. */
		bool RecvFromRobot(std::array<char, 256>& buffer);

	private:
		SOCKET m_CameraTelnetSocket;	// Client SOCKET to give commands to the camera
		SOCKET m_CameraTCPDeviceSocket;	// Client SOCKET to receive the camera's data

		SOCKET m_ChessterListenSocket;	// Server SOCKET to listen for client connections
		SOCKET m_RobotClientSocket;		// Client SOCKET for accepting connection from staubli robot

		static std::array<char, 256> m_CameraDataBuffer;
	};
}

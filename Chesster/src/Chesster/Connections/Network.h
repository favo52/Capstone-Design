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

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#pragma once

#include "Chesster/Connections/Winsock.h"

// Cognex camera login
// User: "admin\r\n"
// Pass: "\r\n"

// Command to take pic: "SE8\r\n"
// if received 1 success
// else error

namespace Chesster
{
	/* Handles the TCP/IP communication between the Cognex camera and the Staubli robot. */
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
			Enables the CHESSTER program to act as a TCP/IP server. This allows the Staubli robot to
			connect as a client and establish TCP/IP communication. */
		static void ChessterServerThread();

		/*	Signals that the Telnet and TCPDevice SOCKETs will not be sending any more data. */
		void ShutdownCamera();

		/*	Closes the Camera Telnet and TCPDevice SOCKETs. */
		void DisconnectCamera();
		
		/*	Closes the Chesster listening SOCKET and the Robot Client SOCKET. */
		void DisconnectRobot();

		/**	Sends a message through TCP/IP to the camera.
		 @param command The message to be sent.
		 @return True if the message was sent succesfully, false otherwise. */
		bool SendToCamera(const std::string& data);

		/**	Sends a message through TCP/IP to the staubli robot.
		 @param command The message to be sent.
		 @return True if the message was sent succesfully, false otherwise. */
		bool SendToRobot(const std::string& data);

		/**	Retrieves the buffer that stores the data received from the Cognex Camera.
		 @return An char array of size 256 containing the piece location on the physical board. */
		static const std::array<char, 256>& GetCameraBuffer() { return m_CameraDataBuffer; }

	private:
		/**	Waits to receive any new data from the specified SOCKET. 
			This function should be used in its own thread so it doesn't lock up the program.
		 @param socket The socket from where the data will be received.
		 @param buffer The buffer where the received data will be stored.
		 @return True if a message was received successfully, false if the communication was closed. */
		template<size_t S>
		bool RecvData(const SOCKET& socket, std::array<char, S>& buffer);

	private:
		SOCKET m_CameraTelnetSocket;	// Client SOCKET to give commands to the camera
		SOCKET m_CameraTCPDeviceSocket;	// Client SOCKET to receive the camera's data

		SOCKET m_ChessterListenSocket;	// Server SOCKET to listen for client connections
		SOCKET m_RobotClientSocket;		// Client SOCKET for accepting connection from staubli robot

		static std::array<char, 256> m_CameraDataBuffer;
	};
}

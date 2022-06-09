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
#include "Chesster/Connections/Winsock.h"

namespace Chesster
{
	// Use Winsock version 2.2
	constexpr WORD WINSOCK_VER{ MAKEWORD(2, 2) };

	Winsock::Winsock() :
		m_WSAData{ NULL }
	{
		// Initialize Winsock
		int iResult = WSAStartup(WINSOCK_VER, &m_WSAData);
		if (iResult != Result::Success) // Ensure system supports Winsock
		{
			LOG_ERROR("WINSOCK: WSAStartup() failed with error: {0}", iResult);
			throw std::runtime_error("Unable to initialize Winsock!");
		}
	}

	Winsock::~Winsock()
	{
		WSACleanup();
	}

	bool Winsock::CreateClientSocket(SOCKET& m_socket, const std::string& ip, const std::string& port)
	{
		// Contains a sockaddr structure
		struct addrinfo* resultPtr, hints;

		// Prepare addrinfo
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;  		// AF_INET for IPv4. AF_INET6 for IPv6.
		hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.

		// Resolve the server address and port
		int iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &resultPtr);
		if (iResult == Result::Failure)
		{
			LOG_ERROR("WINSOCK: getaddrinfo() failed with error: {0}", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (struct addrinfo* ResultPtr = resultPtr; ResultPtr != nullptr; ResultPtr = ResultPtr->ai_next)
		{
			// Create a SOCKET for connecting to server
			m_socket = socket(ResultPtr->ai_family, ResultPtr->ai_socktype, ResultPtr->ai_protocol);
			if (m_socket == INVALID_SOCKET) // Ensure it's a valid socket.
			{
				LOG_ERROR("WINSOCK: socket() failed with error: {0}", WSAGetLastError());
				freeaddrinfo(resultPtr);
				return false;
			}

			// Connect to server.
			iResult = connect(m_socket, ResultPtr->ai_addr, (int)ResultPtr->ai_addrlen);
			if (iResult == SOCKET_ERROR) // Check for general errors.
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		// Not needed anymore, free the memory
		freeaddrinfo(resultPtr);

		if (m_socket == INVALID_SOCKET)
			return false;

		return true;
	}

	bool Winsock::CreateServerSocket(SOCKET& m_socket, const std::string& ip, const std::string& port)
	{
		// Contains a sockaddr structure
		struct addrinfo* resultPtr, hints;

		// Prepare addrinfo
		ZeroMemory(&hints, sizeof(hints));	// ZeroMemory fills the hints with zeros, prepares the memory to be used
		hints.ai_family = AF_INET;  		// AF_INET for IPv4. AF_INET6 for IPv6. AF_UNSPEC for either (might cause error).
		hints.ai_socktype = SOCK_STREAM;	// Used to specify a stream socket.
		hints.ai_protocol = IPPROTO_TCP;	// Used to specify the TCP protocol.
		hints.ai_flags = AI_PASSIVE;		// Indicates the caller intends to use the returned socket address structure
		    								// in a call to the bind function.
		// Resolve the local address and port to be used by the server
		// The getaddrinfo function is used to determine the values in the sockaddr structure
		int iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &resultPtr);
		if (iResult != Result::Success) // Ensure an address was received.
		{
			LOG_ERROR("WINSOCK: getaddrinfo() failed with error: {0}", iResult);
			return false;
		}

		// Create a SOCKET for the server to listen for client connections
		m_socket = socket(resultPtr->ai_family, resultPtr->ai_socktype, resultPtr->ai_protocol);
		if (m_socket == INVALID_SOCKET) // Ensure the socket is valid.
		{
			LOG_ERROR("WINSOCK: socket() failed with error: {0}", WSAGetLastError());
			freeaddrinfo(resultPtr);
			return false;
		}

		// For a server to accept client connections,
		// it must be bound to a network address within the system.

		// Setup the TCP listening socket
		iResult = bind(m_socket, resultPtr->ai_addr, (int)resultPtr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			LOG_ERROR("WINSOCK: bind() socket error: {0}", WSAGetLastError());
			freeaddrinfo(resultPtr);
			closesocket(m_socket);
			return false;
		}

		// Once the bind function is called, the address information
		// returned by the getaddrinfo function is no longer needed
		freeaddrinfo(resultPtr);

		// After the socket is bound to an IP address and port on the system, the server
		// must then listen on that IP address and port for incoming connection requests.
		if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			LOG_ERROR("WINSOCK: listen() socket error: {0}", WSAGetLastError());
			closesocket(m_socket);
			return false;
		}

		return true;
	}

	SOCKET Winsock::AcceptClient(SOCKET& listenSocket)
	{
		return accept(listenSocket, nullptr, nullptr);
	}
}

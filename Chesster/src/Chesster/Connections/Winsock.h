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

// Need to tell the compiler to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

namespace Chesster
{
	/*	Abstraction of Microsoft's Windows Sockets 2.
		Facilitates the creation of TCP server and client sockets. */
	class Winsock
	{
	public:
		/*	Initializes Winsock populating the WSADATA. */
		Winsock();

		/*	Shut down the socket DLL. */
		virtual ~Winsock();

		bool CreateClientSocket(SOCKET& m_socket, const std::string& ip, const std::string& port);
		bool CreateServerSocket(SOCKET& m_socket, const std::string& ip, const std::string& port);

		SOCKET AcceptClient(SOCKET& listenSocket);

	private:
		enum Result { Success, Failure };

	private:
		WSADATA m_WSAData;
	};
}

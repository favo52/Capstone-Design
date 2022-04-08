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

		SOCKET AcceptClient(const SOCKET& listenSocket);

	private:
		enum Result { Success, Failure };

	private:
		WSADATA m_WSAData;
	};
}

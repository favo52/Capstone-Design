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
		Winsock();
		virtual ~Winsock();

		bool CreateClientSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port);
		bool CreateServerSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port);

		SOCKET AcceptClient(const SOCKET& listenSocket);

		void DNSLookup(const SOCKET& m_socket);

	private:
		enum Result { Success, Failure };

	private:
		WSADATA m_WSAData;
		
		sockaddr_in m_SockAddr;	// Socket information
		int m_SockAddrSize;		// Socket size

		// Contains a sockaddr structure
		struct addrinfo* m_Result, * m_Ptr;
	};
}

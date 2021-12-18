#pragma once

// Need to tell the compiler to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// Command to take pic
// User: admin
// Pass: 

// SE8
// if received 1 success
// else error

namespace Chesster
{
	class ClientTCP
	{
	public:
		ClientTCP();
		virtual ~ClientTCP();

		void ConnectSockets();
		void DisconnectSockets();

		bool SendCommand(const std::string command);
		inline const std::string GetData() const { return m_Data; }

	private:
		bool ConnectSocket(SOCKET& m_socket, PCSTR ip, PCSTR port);
		bool RecvBuffer();

		void DNSLookup(const SOCKET& m_socket);

		// Multithreading
		static unsigned int __stdcall ReceiveBuffer(void* data);

	private:
		enum Result
		{
			Success = 0, Failure
		};

	private:
		WSADATA m_WSAData;

		static std::string m_Data;

		SOCKET m_CommandSocket; // Command the camera to take a pic
		static SOCKET m_BufferSocket; // Receive the camera's buffer

		// Declare an addrinfo object that contains a
		// sockaddr structure and initialize these values
		struct addrinfo* result = NULL, *ptr = NULL, hints;
	};
}

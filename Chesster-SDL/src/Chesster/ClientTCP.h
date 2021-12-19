#pragma once

// Need to tell the compiler to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// Cognex camera login
// User: admin
// Pass: 

// Command to take pic
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

		void ConnectCamera();
		void DisconnectCamera();

		bool SendCommand(const std::string& command = "SE8\n");
		bool RecvConfirmation();

		inline const std::string& GetData() const { return m_Data; }

	public:
		static bool DataReceived;

	private:
		bool ConnectSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port);
		bool RecvData();

		void DNSLookup(const SOCKET& m_socket);

		// Multithreading
		static unsigned int __stdcall DataStream(void* data);

	private:
		enum Result
		{
			Success = 0, Failure = 1
		};

	private:
		WSADATA m_WSAData;

		static std::string m_Data;

		SOCKET m_CommandSocket; // Command the camera to take a pic
		static SOCKET m_BufferSocket; // Receive the camera's data

		// Declare an addrinfo object that contains a
		// sockaddr structure and initialize these values
		struct addrinfo* result = NULL, *ptr = NULL, hints;
	};
}

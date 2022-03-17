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
	/// <summary>
	/// 
	/// </summary>
	class TCPConnection
	{
	public:
		TCPConnection();
		virtual ~TCPConnection();

		// Camera connection
		void ConnectCamera();
		void DisconnectCamera();

		// Robot connection
		static unsigned int __stdcall ConnectRobotThread(void* data);
		void DisconnectRobot();

		bool SendCameraCommand(const std::string& command);
		bool RecvCameraConfirmation();

		bool SendToRobot(const std::string& command);
		bool RecvFromRobot();

		const std::string& GetCameraData() const { return m_CameraData; }
		const std::string& GetRobotData() const { return m_RobotData; }

		static TCPConnection& Get() { return *s_Instance; }

	public:
		static bool CameraDataReceived;
		static bool RobotDataReceived;

		static bool IsCameraStreaming;
		static bool IsServerListening;

	private:
		bool CreateClientSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port);
		bool CreateServerSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port);
		
		void DNSLookup(const SOCKET& m_socket);

		bool RecvCameraData();
		static unsigned int __stdcall CameraDataStreamThread(void* data);

	private:
		enum Result
		{
			Success = 0, Failure = 1
		};

	private:
		WSADATA m_WSAData;

		static std::string m_CameraData;
		static std::string m_RobotData;

		SOCKET m_CameraCommandSocket; // Command the camera to take a pic
		SOCKET m_CameraBufferSocket; // Receive the camera's data

		SOCKET m_RobotListenSocket; // SOCKET for Server to listen for Client connections
		SOCKET m_RobotClientSocket; // SOCKET for accepting connections from clients

		// Socket information
		sockaddr_in m_SockAddr{ NULL };
		int m_SockAddrSize{ sizeof(m_SockAddr) };

		// Declare an addrinfo object that contains a
		// sockaddr structure and initialize these values
		struct addrinfo* result{ nullptr }, *ptr{ nullptr }, hints;

		static TCPConnection* s_Instance; // Pointer to this
	};
}

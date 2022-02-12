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

		bool ConnectRobot();
		void DisconnectRobot();

		bool SendCameraCommand(const std::string& command);
		bool RecvCameraConfirmation();

		bool SendRobotCommand(const std::string& command);
		bool RecvRobotConfirmation();

		const std::string& GetCameraData() const { return m_CameraData; }
		const std::string& GetRobotData() const { return m_RobotData; }

	public:
		static bool CameraDataReceived;
		static bool RobotDataReceived;

	private:
		bool ConnectSocket(SOCKET& m_socket, const PCSTR& ip, const PCSTR& port);
		void DNSLookup(const SOCKET& m_socket);

		bool RecvCameraData();
		bool RecvRobotData();

		// Multithreading
		static unsigned int __stdcall CameraDataStream(void* data);
		static unsigned int __stdcall RobotDataStream(void* data);

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
		static SOCKET m_CameraBufferSocket; // Receive the camera's data

		SOCKET m_RobotCommandSocket;
		static SOCKET m_RobotLogSocket;

		// Declare an addrinfo object that contains a
		// sockaddr structure and initialize these values
		struct addrinfo* result{ nullptr }, *ptr{ nullptr }, hints;
	};
}

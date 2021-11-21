#pragma once

#include "pch.h"

// Uses Windows types because "When in Rome..."

namespace Chesster
{
	constexpr DWORD BUFSIZE{ 2048 };

	class Connector
	{
	public:
		Connector();
		~Connector();

		void ConnectToEngine(LPWSTR path);
		std::string getNextMove(const std::string& position);

		std::vector<std::string> GetValidMoves(const std::string&);

	private:
		void CloseConnections();

	private:
		STARTUPINFO m_StartInfo;
		SECURITY_ATTRIBUTES m_SecAttr;
		PROCESS_INFORMATION m_ProcessInfo;
		HANDLE m_Pipe_IN_Rd, m_Pipe_IN_Wr, m_Pipe_OUT_Rd, m_Pipe_OUT_Wr;
		BYTE m_Buffer[BUFSIZE];
		DWORD m_Read, m_Written;
		BOOL m_Success;
	};
}

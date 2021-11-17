#include "Connector.h"
#include "assert.h"

namespace Chesster
{
	Connector::Connector() :
		m_StartInfo{ 0 },
		m_SecAttr{ 0 },
		m_ProcessInfo{ 0 },
		m_Pipe_IN_Wr{ nullptr },
		m_Pipe_IN_Rd{ nullptr },
		m_Pipe_OUT_Wr{ nullptr },
		m_Pipe_OUT_Rd{ nullptr },
		m_Read{ 0 },
		m_Written{ 0 },
		m_Success{ FALSE }
	{
		// Set up members of the PROCESS_INFORMATION structure.
		ZeroMemory(&m_ProcessInfo, sizeof(STARTUPINFO));

		// Fill buffer with zeros
		ZeroMemory(m_Buffer, sizeof(m_Buffer));
	}

	Connector::~Connector()
	{
		CloseConnections();
	}

	void Connector::ConnectToEngine(LPWSTR path)
	{
		// Set the bInheritHandle flag so pipe handles are inherited.
		m_SecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		m_SecAttr.bInheritHandle = TRUE;
		m_SecAttr.lpSecurityDescriptor = NULL;

		// Create pipes for the child's processes
		if (!CreatePipe(&m_Pipe_OUT_Rd, &m_Pipe_OUT_Wr, &m_SecAttr, 0))
			throw std::runtime_error("CreatePipe OUT failed.");

		if (!CreatePipe(&m_Pipe_IN_Rd, &m_Pipe_IN_Wr, &m_SecAttr, 0))
			throw std::runtime_error("CreatePipe IN failed.");

		// Set up members of the STARTUPINFO structure.
		ZeroMemory(&m_StartInfo, sizeof(STARTUPINFO));
		m_StartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		m_StartInfo.wShowWindow = SW_HIDE;
		m_StartInfo.hStdInput = m_Pipe_IN_Rd;
		m_StartInfo.hStdOutput = m_Pipe_OUT_Wr;
		m_StartInfo.hStdError = m_Pipe_OUT_Wr;

		// Create the child process
		m_Success = CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &m_StartInfo, &m_ProcessInfo);
		if (!m_Success)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return;
		}

		//// Check if Engine is ready
		m_Success = WriteFile(m_Pipe_IN_Wr, "isready\n", sizeof("isready\n"), &m_Written, NULL);
		//if (!m_Success)
		//{
		//	printf("WriteFile \"isready\" failed (%d).\n", GetLastError());
		//	return;
		//}
		Sleep(500);

		// Read Engine's response
		std::string msg;
		do
		{
			ZeroMemory(m_Buffer, sizeof(m_Buffer));
			m_Success = ReadFile(m_Pipe_OUT_Rd, m_Buffer, sizeof(m_Buffer), &m_Read, NULL);
			if (!m_Success || m_Read == 0) break;
			msg += (char*)m_Buffer;
			std::cout << msg;
		} while (m_Read >= sizeof(m_Buffer));
	}

	std::string Connector::getNextMove(std::string chessMove)
	{
		std::string computerMove;
		chessMove = "position startpos moves " + chessMove + "\ngo\n";

		WriteFile(m_Pipe_IN_Wr, chessMove.c_str(), chessMove.length(), &m_Written, NULL);
		Sleep(500);

		PeekNamedPipe(m_Pipe_OUT_Rd, m_Buffer, sizeof(m_Buffer), &m_Read, &m_BytesAvailable, NULL);
		do
		{
			ZeroMemory(m_Buffer, sizeof(m_Buffer));
			m_Success = ReadFile(m_Pipe_OUT_Rd, m_Buffer, sizeof(m_Buffer), &m_Read, NULL);
			if (!m_Success || m_Read == 0) break;

			//m_Buffer[m_Read] = 0;
			computerMove += (char*)m_Buffer;
			std::cout << "Stockfish: " << computerMove;
		} while (m_Read >= sizeof(m_Buffer));

		int n = computerMove.find("bestmove");
		if (n != -1) // if it's found
			return computerMove.substr(n + 9, 4); // subtract "bestmove ", grab next 4

		return "error";
	}

	void Connector::CloseConnections()
	{
		WriteFile(m_Pipe_IN_Wr, "quit\n", 5, &m_Written, NULL);

		// Close process and thread handles.
		if (m_Pipe_IN_Wr != NULL) CloseHandle(m_Pipe_IN_Wr);
		if (m_Pipe_IN_Rd != NULL) CloseHandle(m_Pipe_IN_Rd);
		if (m_Pipe_OUT_Wr != NULL) CloseHandle(m_Pipe_OUT_Wr);
		if (m_Pipe_OUT_Rd != NULL) CloseHandle(m_Pipe_OUT_Rd);
		CloseHandle(m_ProcessInfo.hProcess);
		CloseHandle(m_ProcessInfo.hThread);

		std::cout << "Engine connection closed.\n";
	}
}

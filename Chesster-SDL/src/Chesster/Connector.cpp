#include "pch.h"

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "Connector.h"

// "... do as the Romans do."

namespace Chesster
{
	Connector::Connector() :
		// Chess engine
		m_StartInfo{ 0 },
		m_SecAttr{ 0 },
		m_ProcessInfo{ 0 },
		m_Pipe_IN_Wr{ nullptr },
		m_Pipe_IN_Rd{ nullptr },
		m_Pipe_OUT_Wr{ nullptr },
		m_Pipe_OUT_Rd{ nullptr },
		m_Read{ 0 },
		m_Written{ 0 },
		m_Success{ FALSE },

		// Python script
		m_StartInfo_Py{ 0 },
		m_ProcessInfo_Py{ 0 },
		m_Success_Py{ FALSE }
	{
		// Set up members of the PROCESS_INFORMATION structure.
		ZeroMemory(&m_ProcessInfo, sizeof(STARTUPINFO));

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

		// Setup buffer
		ZeroMemory(m_Buffer, BUFSIZE);

		//*****************************************************
		//	PYTHON SCRIPT INFO
		//*****************************************************

		// Set up members of the STARTUPINFO structure.
		ZeroMemory(&m_StartInfo_Py, sizeof(STARTUPINFOA));
		m_StartInfo_Py.cb = sizeof(m_StartInfo_Py);
		m_StartInfo_Py.dwFlags = STARTF_USESHOWWINDOW;
		m_StartInfo_Py.wShowWindow = SW_HIDE;
		
		// Set up members of the PROCESS_INFORMATION structure.
		ZeroMemory(&m_ProcessInfo_Py, sizeof(PROCESS_INFORMATION));

		Py_Initialize();
	}

	Connector::~Connector()
	{
		CloseConnections();
		Py_Finalize();
	}

	void Connector::ConnectToEngine(LPWSTR path)
	{
		// Create the child process
		m_Success = CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &m_StartInfo, &m_ProcessInfo);
		if (!m_Success)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return;
		}

		// Check if engine is ready
		CHAR str[] = "uci\nisready\n";
		m_Success = WriteFile(m_Pipe_IN_Wr, str, strlen(str), &m_Written, NULL);
		Sleep(150);

		// Set difficulty level
		CHAR setSkillLevel[] = "setoption name Skill Level value 0\n";
		WriteFile(m_Pipe_IN_Wr, setSkillLevel, strlen(setSkillLevel), &m_Written, NULL);
		CHAR setLimit[] = "setoption name UCI_LimitStrength value true\n";
		WriteFile(m_Pipe_IN_Wr, setLimit, strlen(setLimit), &m_Written, NULL);
		Sleep(150);

		// Read engine's reply
		std::string msg;
		do
		{
			ZeroMemory(m_Buffer, BUFSIZE);
			m_Success = ReadFile(m_Pipe_OUT_Rd, m_Buffer, BUFSIZE, &m_Read, NULL);
			if (!m_Success || m_Read == 0) break;

			msg += (char*)m_Buffer;

		} while (m_Read >= sizeof(m_Buffer));
		std::cout << msg;
	}

	std::string Connector::GetNextMove(const std::string& moveHistory)
	{
		std::string msg = { "position startpos moves " + moveHistory + "\ngo depth 1\n" };

		// Send position to engine
		WriteFile(m_Pipe_IN_Wr, msg.c_str(), msg.length(), &m_Written, NULL);
		Sleep(150);
		msg.clear();

		// Read engine's reply
		do
		{
			ZeroMemory(m_Buffer, BUFSIZE);
			m_Success = ReadFile(m_Pipe_OUT_Rd, m_Buffer, BUFSIZE, &m_Read, NULL);
			if (!m_Success || m_Read == 0) break;

			msg += (char*)m_Buffer;

		} while (m_Read >= sizeof(m_Buffer));
		std::cout << msg;

		// Grab the engine's move
		int found = msg.find("bestmove");
		if (found != std::string::npos)
			// Subtract "bestmove ", grab next 4 (the notation, ex. d2d4)
			return msg.substr(found + 9, 4);

		return "error"; // If no bestmove is found
	}

	// Run the Python script
	std::vector<std::string> Connector::GetValidMoves(const std::string& path, const std::string& fen)
	{
		std::string arg = { path + " " + fen };

		// Convert std::string into LPSTR
		LPSTR lpPath = const_cast<char*>(path.c_str());
		LPSTR lpPathArg = const_cast<char*>(arg.c_str());

		// Create the child process
		m_Success_Py = CreateProcessA(lpPath, lpPathArg, NULL, NULL, FALSE, 0, NULL, NULL, &m_StartInfo_Py, &m_ProcessInfo_Py);
		if (!m_Success_Py)
		{
			printf("CreateProcessA failed (%d).\n", GetLastError());
			throw std::runtime_error("Unable to run Python script");
		}

		// Wait for script to finish
		WaitForSingleObject(m_ProcessInfo_Py.hProcess, INFINITE);

		// Close connection
		CloseHandle(m_ProcessInfo_Py.hProcess);
		CloseHandle(m_ProcessInfo_Py.hThread);

		// Read all moves from file and store inside std::vector
		std::string filename{ "validmoves.txt" };
		std::ifstream ifs{ filename };
		if (!ifs) throw std::runtime_error("Unable to open file " + filename);
		
		std::vector<std::string> validMoves;
		for (std::string move; ifs >> move; )
			validMoves.push_back(move);

		return validMoves;
	}

	//std::vector<std::string> Connector::GetValidMoves(const std::string& fen)
	//{
	//	std::string filename{ "validmoves.txt" };

	//	// Create file with all valid moves using python script
	//	std::string PyCode
	//	{
	//		"from Chessnut import Game\n"
	//		"chessgame = Game()\n"
	//		"chessgame.set_fen(\"" + fen + "\")\n"
	//		"move_list = chessgame.get_moves()\n"
	//		"file = open(\"" + filename + "\", \"w\")\n"
	//		"for element in move_list:"
	//		"	file.write(element + \" \")\n"
	//		"file.close()\n"
	//	};

	//	PyRun_SimpleString(PyCode.c_str());

	//	// Read all moves from file and store inside std::vector
	//	std::ifstream ifs{ filename };
	//	if (!ifs) throw std::runtime_error("Unable to open file " + filename);
	//	std::vector<std::string> validMoves;
	//	for (std::string move; ifs >> move; )
	//		validMoves.push_back(move);

	//	return validMoves;
	//}

	std::string Connector::GetFEN(const std::string& moveHistory)
	{
		std::string msg = { "position startpos moves " + moveHistory + "\nd\n" };

		// Send position to engine
		WriteFile(m_Pipe_IN_Wr, msg.c_str(), msg.length(), &m_Written, NULL);
		Sleep(150);

		msg.clear();
		// Read engine's reply
		do
		{
			ZeroMemory(m_Buffer, BUFSIZE);
			m_Success = ReadFile(m_Pipe_OUT_Rd, m_Buffer, BUFSIZE, &m_Read, NULL);
			if (!m_Success || m_Read == 0) break;

			msg += (char*)m_Buffer;

		} while (m_Read >= sizeof(m_Buffer));
		std::cout << msg;

		// Grab the FEN string
		int found = msg.find("Fen:");
		if (found != std::string::npos)
		{
			// Erase everything up to and including "Fen:"
			msg.erase(0, found + sizeof("Fen:"));

			// Grab everything until "Key:" is reached
			std::istringstream iss{ msg };
			msg.clear();
			for (std::string str; iss >> str; )
			{
				if (str == "Key:")
				{
					msg.pop_back(); // delete last whitespace
					return msg;
				}
				msg += str + ' ';
			}
		}

		return "error"; // If no bestmove is found
	}

	void Connector::CloseConnections()
	{
		// Quit chess engine
		CHAR str[] = "quit\n";
		WriteFile(m_Pipe_IN_Wr, str, strlen(str), &m_Written, NULL);

		// Close process and thread handles.
		CloseHandle(m_Pipe_IN_Wr);
		CloseHandle(m_Pipe_IN_Rd);
		CloseHandle(m_Pipe_OUT_Wr);
		CloseHandle(m_Pipe_OUT_Rd);
		CloseHandle(m_ProcessInfo.hProcess);
		CloseHandle(m_ProcessInfo.hThread);

		std::cout << "Engine connection closed.\n";
	}
}

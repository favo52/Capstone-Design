#include "pch.h"
#include "Chesster/Connections/Interprocess.h"

#include "Chesster/Layers/GameLayer.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// "... do as the Romans do."

namespace Chesster
{
	Interprocess::Interprocess() :
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

	Interprocess::~Interprocess()
	{
		CloseAllConnections();
		Py_Finalize();
	}

	void Interprocess::ConnectToEngine(const LPWSTR& path)
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		// Create the child process
		m_Success = CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &m_StartInfo, &m_ProcessInfo);
		if (!m_Success)
		{
			LOG_ERROR("CreateProcess failed with error: {0}", GetLastError());
			return;
		}
		LOG_INFO("Engine connection opened.");

		std::string msg = GetEngineReply();
		LOG_INFO(msg);
		consolePanel->AddLog(msg.c_str());

		// Check if engine is ready
		CHAR str[] = "uci\nucinewgame\nisready\n";
		m_Success = WriteFile(m_Pipe_IN_Wr, str, strlen(str), &m_Written, NULL);
		Sleep(150);

		// Set default difficulty level
		CHAR setSkillLevel[] = "setoption name Skill Level value 0\n";
		m_Success = WriteFile(m_Pipe_IN_Wr, setSkillLevel, strlen(setSkillLevel), &m_Written, NULL);
		Sleep(150);

		msg = { "\n" + GetEngineReply() };
		LOG_INFO(msg);
		consolePanel->AddLog(msg.c_str());
	}

	void Interprocess::ResetGame()
	{
		// The fen is the chess starting position.
		CHAR str[] = "ucinewgame\nposition fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\nd\nisready\n";
		m_Success = WriteFile(m_Pipe_IN_Wr, str, strlen(str), &m_Written, NULL);
		Sleep(150);

		// Read engine's reply and print it
		std::string msg{ "GAME RESET\n" };
		msg += GetEngineReply() + '\n';
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg.c_str());
	}

	void Interprocess::EvaluateGame()
	{
		CHAR str[] = "eval\n";
		m_Success = WriteFile(m_Pipe_IN_Wr, str, strlen(str), &m_Written, NULL);
		Sleep(150);

		// Read engine's reply and print it
		std::string msg{ "GAME EVALUATION\n" };
		msg += GetEngineReply() + '\n';
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg.c_str());
	}

	void Interprocess::SetDifficultyLevel(int difficulty)
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		std::string skill = { "setoption name Skill Level value " + std::to_string(difficulty) + "\n" };
		WriteFile(m_Pipe_IN_Wr, skill.c_str(), skill.length(), &m_Written, NULL);
		Sleep(150);

		std::string msg = { "Difficulty set to skill level " + std::to_string(difficulty) + ".\n" };
		LOG_INFO(msg);
		consolePanel->AddLog(msg.c_str());
	}

	// Overrides skill level
	void Interprocess::SetDifficultyELO(int elo)
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		// Specify ELO
		std::string setElo = { "setoption name UCI_Elo value " + std::to_string(elo) + "\n" };
		WriteFile(m_Pipe_IN_Wr, setElo.c_str(), setElo.length(), &m_Written, NULL);
		Sleep(100);

		std::string msg = { "Difficulty set to " + std::to_string(elo) + " ELO.\n" };
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg.c_str());
	}

	void Interprocess::ToggleELO(bool boolean)
	{
		std::string toggle{ "false\n" };
		if (boolean) toggle = { "true\n" };

		std::string toogleELO = { "setoption name UCI_LimitStrength value " + toggle };
		WriteFile(m_Pipe_IN_Wr, toogleELO.c_str(), toogleELO.length() , &m_Written, NULL);
		Sleep(100);

		std::string boolMsg = (boolean) ? "activated" : "deactivated";
		std::string msg = { "ELO Rating " + boolMsg + ".\n" };
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg.c_str());
	}

	std::string Interprocess::GetNextMove(const std::string& moveHistory)
	{
		std::string msg = { "position startpos moves " + moveHistory + "\ngo depth 10\nd\n" };

		// Send position to engine
		WriteFile(m_Pipe_IN_Wr, msg.c_str(), msg.length(), &m_Written, NULL);
		Sleep(150);

		// Read engine's reply
		msg = GetEngineReply() + '\n';
		
		// Grab the engine's move
		int found = msg.find("bestmove");
		if (found != std::string::npos)
		{
			// Erase everything up to and including "bestmove"
			msg.erase(0, found + sizeof("bestmove"));
			std::istringstream iss{ msg };
			msg.clear();
			iss >> msg; // grab the move

			return msg;
		}
		
		return "error"; // If no bestmove is found
	}

	// Run the Python script
	std::vector<std::string> Interprocess::GetValidMoves(const std::string& path, const std::string& fen)
	{
		std::string argument = { path + " " + fen };

		// Convert std::string into LPSTR
		LPSTR lpPath = const_cast<char*>(path.c_str());
		LPSTR lpArgument = const_cast<char*>(argument.c_str());

		// Create python script child process
		m_Success_Py = CreateProcessA(lpPath, lpArgument, NULL, NULL, FALSE, 0, NULL, NULL, &m_StartInfo_Py, &m_ProcessInfo_Py);
		if (!m_Success_Py)
		{
			LOG_ERROR("CreateProcessA failed with error: {0}", GetLastError());
			throw std::runtime_error("Unable to run Python script.");
		}

		// Wait for script to finish
		WaitForSingleObject(m_ProcessInfo_Py.hProcess, INFINITE);

		// Close python script child process
		CloseHandle(m_ProcessInfo_Py.hProcess);
		CloseHandle(m_ProcessInfo_Py.hThread);

		// Attempt to open the created/updated file
		const std::string filename{ "validmoves.txt" };
		std::ifstream ifs{ filename };
		if (!ifs) throw std::runtime_error("Unable to open file " + filename); // Error checking
		
		// Read all moves from the file and store inside a std::vector to return it
		std::vector<std::string> validMoves;
		for (std::string move; ifs >> move; )
			validMoves.push_back(move);

		return validMoves;
	}

	std::string Interprocess::GetFEN(const std::string& moveHistory)
	{
		std::string msg = { "position startpos moves " + moveHistory + "\nd\n" };

		// Send position to engine
		WriteFile(m_Pipe_IN_Wr, msg.c_str(), msg.length(), &m_Written, NULL);
		Sleep(150);

		// Read engine's reply and print it
		msg = GetEngineReply() + '\n';
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg.c_str());
		msg.pop_back();

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
					return msg;		// only the FEN notation is left, return it
				}
				msg += str + ' ';
			}
		}

		return "error"; // If no FEN string is found
	}

	const std::string Interprocess::GetEngineReply()
	{
		// Read engine's reply
		std::string msg{};
		do
		{
			ZeroMemory(m_Buffer, BUFSIZE);
			m_Success = ReadFile(m_Pipe_OUT_Rd, m_Buffer, BUFSIZE, &m_Read, NULL);
			if (!m_Success || m_Read == 0) break;

			msg += (char*)m_Buffer;

		} while (m_Read >= sizeof(m_Buffer));
		
		return msg;
	}

	void Interprocess::CloseAllConnections()
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

		LOG_INFO("Engine connection closed.");
	}
}

#include "pch.h"
#include "Chesster/Connections/ChessEngine.h"

#include "Chesster/Layers/GameLayer.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// "... do as the Romans do."

namespace Chesster
{
	ChessEngine::ChessEngine() :
		m_StartInfo{ 0 },
		m_SecAttr{ 0 },
		m_ProcessInfo{ 0 },
		m_WritePipe_IN{ nullptr },
		m_ReadPipe_IN{ nullptr },
		m_WritePipe_OUT{ nullptr },
		m_ReadPipe_OUT{ nullptr }
	{
		// Set up members of the PROCESS_INFORMATION structure.
		ZeroMemory(&m_ProcessInfo, sizeof(PROCESS_INFORMATION));

		// Set the bInheritHandle flag so pipe handles are inherited.
		m_SecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		m_SecAttr.bInheritHandle = TRUE;
		m_SecAttr.lpSecurityDescriptor = NULL;

		// Create pipes for the child's processes
		if (!CreatePipe(&m_ReadPipe_OUT, &m_WritePipe_OUT, &m_SecAttr, 0))
			throw std::runtime_error("CreatePipe OUT failed.");

		if (!CreatePipe(&m_ReadPipe_IN, &m_WritePipe_IN, &m_SecAttr, 0))
			throw std::runtime_error("CreatePipe IN failed.");

		// Set up members of the STARTUPINFO structure.
		ZeroMemory(&m_StartInfo, sizeof(STARTUPINFO));
		m_StartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		m_StartInfo.wShowWindow = SW_HIDE;
		m_StartInfo.hStdInput = m_ReadPipe_IN;
		m_StartInfo.hStdOutput = m_WritePipe_OUT;
		m_StartInfo.hStdError = m_WritePipe_OUT;

		Py_Initialize();
	}

	ChessEngine::~ChessEngine()
	{
		CloseAllConnections();
		Py_Finalize();
	}

	void ChessEngine::ConnectToEngine(const LPWSTR& path)
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		// Create the child process (runs the chess engine's .exe file)
		BOOL success = CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &m_StartInfo, &m_ProcessInfo);
		if (!success)
		{
			LOG_ERROR("CreateProcess failed with error: {0}", GetLastError());
			return;
		}
		LOG_INFO("Engine connection opened.");

		std::string msg = GetEngineReply();
		LOG_INFO(msg);
		consolePanel->AddLog(msg);

		// Check if engine is ready
		if (!WriteToEngine("uci\nucinewgame\nisready\n"))
			LOG_WARN("Unable to check if engine is ready.");

		// Set default difficulty level
		SetDifficultyLevel();
		SetDifficultyELO();
		
		Sleep(100);
		msg = { "\n" + GetEngineReply() };
		LOG_INFO(msg);
		consolePanel->AddLog(msg);
	}

	void ChessEngine::NewGame()
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		// The fen is the chess starting position.
		const std::string str{ "ucinewgame\nposition fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\nd\nisready\n" };
		if (!WriteToEngine(str))
		{
			std::string errorMsg{ "Failed to send \"ucinewgame\" request." };
			LOG_WARN(errorMsg);
			consolePanel->AddLog(errorMsg);
			return;
		}
		Sleep(200);

		// Read engine's reply and print it
		std::string msg{ "GAME RESET\n" };
		msg += GetEngineReply() + '\n';
		LOG_INFO(msg);
		consolePanel->AddLog(msg);
	}

	void ChessEngine::EvaluateGame()
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		const std::string str{ "eval\n" };
		if (!WriteToEngine(str))
		{
			std::string errorMsg{ "Failed to send \"eval\" request." };
			LOG_WARN(errorMsg);
			consolePanel->AddLog(errorMsg);
			return;
		}
		Sleep(100);

		// Read engine's reply and print it
		std::string msg{ "GAME EVALUATION\n" };
		msg += GetEngineReply() + '\n';
		LOG_INFO(msg);
		consolePanel->AddLog(msg);
	}

	void ChessEngine::SetDifficultyLevel(int difficulty)
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		const std::string skill = { "setoption name Skill Level value " + std::to_string(difficulty) + "\n" };
		if (!WriteToEngine(skill))
		{
			LOG_WARN("Unable to set default Skill Level to {0}.", difficulty);
			return;
		}
		Sleep(100);

		std::string msg = { "Difficulty set to Skill Level " + std::to_string(difficulty) + ".\n" };
		LOG_INFO(msg);
		consolePanel->AddLog(msg);
	}

	// Overrides skill level
	void ChessEngine::SetDifficultyELO(int elo)
	{
		ConsolePanel* consolePanel = GameLayer::Get().GetConsolePanel();

		// Specify ELO
		std::string setElo = { "setoption name UCI_Elo value " + std::to_string(elo) + "\n" };
		if (!WriteToEngine(setElo))
		{
			LOG_WARN("Unable to set default ELO Rating to {0}.", elo);
			return;
		}
		Sleep(100);

		std::string msg = { "Difficulty set to " + std::to_string(elo) + " ELO Rating.\n" };
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg);
	}

	void ChessEngine::ToggleELO(bool boolean)
	{
		std::string toggle{ "false\n" };
		if (boolean) toggle = { "true\n" };

		const std::string toogleELO = { "setoption name UCI_LimitStrength value " + toggle };
		if (!WriteToEngine(toogleELO))
		{
			LOG_WARN("Unable to toggle ELO to {0}.", boolean);
			return;
		}
		Sleep(150);

		const std::string boolMsg = (boolean) ? "activated" : "deactivated";
		const std::string msg = { "ELO Rating " + boolMsg + ".\n" };
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg);
	}

	std::string ChessEngine::GetNextMove(const std::string& moveHistory)
	{
		std::string msg = { "position startpos moves " + moveHistory + "\ngo depth 5\nd\n" };
		LOG_INFO(msg);

		// Send position to engine
		if (!WriteToEngine(msg))
		{
			LOG_WARN("Unable to get chess engine's next move.");
			return "error";
		}
		Sleep(200);
		
		// Read engine's reply
		msg = GetEngineReply() + '\n';
		LOG_INFO(msg);

		// Grab the engine's move
		size_t found = msg.find("bestmove");
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

	// Runs a Python script
	std::vector<std::string> ChessEngine::GetValidMoves(const std::string& fen)
	{
		const std::string path{ "assets/script/__init__.exe" };
		const std::string argument = { path + " " + fen };

		// Convert std::string into LPSTR
		LPSTR lpPath = const_cast<char*>(path.c_str());
		LPSTR lpArgument = const_cast<char*>(argument.c_str());

		// Set up members of the STARTUPINFO structure.
		STARTUPINFOA startInfo_Py;
		ZeroMemory(&startInfo_Py, sizeof(STARTUPINFOA));
		startInfo_Py.cb = sizeof(startInfo_Py);
		startInfo_Py.dwFlags = STARTF_USESHOWWINDOW;
		startInfo_Py.wShowWindow = SW_HIDE;

		// Set up members of the PROCESS_INFORMATION structure.
		PROCESS_INFORMATION processInfo_Py;
		ZeroMemory(&processInfo_Py, sizeof(PROCESS_INFORMATION));

		// Run the python script executable as a child process
		BOOL success = CreateProcessA(lpPath, lpArgument, NULL, NULL, FALSE, 0, NULL, NULL, &startInfo_Py, &processInfo_Py);
		if (!success)
		{
			LOG_ERROR("CreateProcessA failed with error: {0}", GetLastError());
			throw std::runtime_error("Unable to run Python script.");
		}

		// Wait for script to finish
		WaitForSingleObject(processInfo_Py.hProcess, INFINITE);

		// Close python script child process
		CloseHandle(processInfo_Py.hProcess);
		CloseHandle(processInfo_Py.hThread);

		// Attempt to open the newly created/updated file
		const std::string filename{ "validmoves.txt" };
		std::ifstream ifs{ filename };
		if (!ifs) throw std::runtime_error("Unable to open file " + filename);
		
		// Read all moves from the file and store inside a std::vector to return it
		std::vector<std::string> validMoves;
		for (std::string move; ifs >> move; )
			validMoves.push_back(move);

		return validMoves;
	}

	std::string ChessEngine::GetFEN(const std::string& moveHistory)
	{
		std::string msg = { "position startpos moves " + moveHistory + "\nd\n" };

		// Send position to engine
		if (!WriteToEngine(msg))
		{
			LOG_ERROR("Unable to get FEN from engine.");
			return "error";
		}
		Sleep(200);

		// Read engine's reply and print it
		msg = GetEngineReply() + '\n';
		LOG_INFO(msg);
		GameLayer::Get().GetConsolePanel()->AddLog(msg);
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

	void ChessEngine::CloseAllConnections()
	{
		// Quit chess engine
		const std::string message{ "quit\n" };
		WriteToEngine(message);

		// Close process and thread handles.
		CloseHandle(m_WritePipe_IN);
		CloseHandle(m_ReadPipe_IN);
		CloseHandle(m_WritePipe_OUT);
		CloseHandle(m_ReadPipe_OUT);
		CloseHandle(m_ProcessInfo.hProcess);
		CloseHandle(m_ProcessInfo.hThread);

		LOG_INFO("Engine connection closed.");
	}

	bool ChessEngine::WriteToEngine(const std::string& message)
	{
		size_t bytesToWrite = message.size();
		DWORD bytesWritten{ 0 };

		while (bytesToWrite > 0)
		{
			BOOL success = WriteFile(m_WritePipe_IN, message.c_str(), bytesToWrite, &bytesWritten, NULL);
			if (!success) { LOG_ERROR("Unable to write \"{0}\" to chess engine.", message); return false; }

			bytesToWrite -= bytesWritten;
		}

		return true;
	}

	const std::string ChessEngine::GetEngineReply()
	{
		std::array<char, 2048> buffer = {};
		std::string msg{};
		DWORD bytesRead{ 0 };

		// Read engine's reply
		do
		{
			buffer = {};
			BOOL success = ReadFile(m_ReadPipe_OUT, buffer.data(), buffer.size(), &bytesRead, NULL);
			if (!success || bytesRead == 0) break;

			msg += buffer.data();
		} while (bytesRead >= sizeof(buffer));

		return msg;
	}
}

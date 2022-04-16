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
		// Set the bInheritHandle flag so pipe handles are inherited.
		m_SecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		m_SecAttr.bInheritHandle = TRUE;
		m_SecAttr.lpSecurityDescriptor = NULL;

		// Create pipes for the child's processes
		if (!CreatePipe(&m_ReadPipe_OUT, &m_WritePipe_OUT, &m_SecAttr, 0))
			throw std::runtime_error("CreatePipe OUT failed.");

		if (!CreatePipe(&m_ReadPipe_IN, &m_WritePipe_IN, &m_SecAttr, 0))
			throw std::runtime_error("CreatePipe IN failed.");

		Py_Initialize();
	}

	ChessEngine::~ChessEngine()
	{
		// Quit chess engine
		WriteToEngine("quit\n");

		// Close process and thread handles.
		CloseHandle(m_WritePipe_IN);
		CloseHandle(m_ReadPipe_IN);
		CloseHandle(m_WritePipe_OUT);
		CloseHandle(m_ReadPipe_OUT);
		CloseHandle(m_ProcessInfo.hProcess);
		CloseHandle(m_ProcessInfo.hThread);

		LOG_INFO("Engine connection closed.");

		Py_Finalize();
	}

	void ChessEngine::ConnectToEngine(const LPWSTR& path)
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		// Set up members of the PROCESS_INFORMATION structure.
		ZeroMemory(&m_ProcessInfo, sizeof(PROCESS_INFORMATION));

		// Set up members of the STARTUPINFO structure.
		ZeroMemory(&m_StartInfo, sizeof(STARTUPINFO));
		m_StartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		m_StartInfo.wShowWindow = SW_HIDE;
		m_StartInfo.hStdInput = m_ReadPipe_IN;
		m_StartInfo.hStdOutput = m_WritePipe_OUT;
		m_StartInfo.hStdError = m_WritePipe_OUT;

		// Create the child process (runs the chess engine's .exe file)
		BOOL success = CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &m_StartInfo, &m_ProcessInfo);
		if (!success)
		{
			LOG_ERROR("CreateProcess failed with error: {0}", GetLastError());
			return;
		}
		LOG_INFO("Engine connection opened.");
		consolePanel.AddLog("Engine connection opened.");

		// Get the engine's welcome message
		std::string engineMessage = ReadFromEngine();
		LOG_INFO(engineMessage);
		consolePanel.AddLog(engineMessage);

		SetDifficultyLevel();
		SetDifficultyELO();

		NewGame();
	}

	void ChessEngine::NewGame()
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		// The fen is the chess starting position.
		if (!WriteToEngine("ucinewgame\nposition fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\nd\nisready\n"))
		{
			const std::string errorMsg{ "Failed to send \"ucinewgame\" request." };
			LOG_WARN(errorMsg);
			consolePanel.AddLog(errorMsg);
			return;
		}

		// Read engine's reply and print it
		std::string engineMessage{ "NEW GAME\n" };
		engineMessage += ReadFromEngine();
		LOG_INFO(engineMessage);
		consolePanel.AddLog("\n" + engineMessage);
	}

	void ChessEngine::EvaluateGame()
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		if (!WriteToEngine("eval\n"))
		{
			const std::string errorMsg{ "Failed to send \"eval\" request." };
			LOG_WARN(errorMsg);
			consolePanel.AddLog(errorMsg);
			return;
		}

		// Read engine's reply and print it
		std::string engineMessage{ "GAME EVALUATION\n" };
		engineMessage += ReadFromEngine();
		LOG_INFO(engineMessage);
		consolePanel.AddLog("\n" + engineMessage);
	}

	void ChessEngine::SetDifficultyLevel(int difficulty)
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		if (!WriteToEngine("setoption name Skill Level value " + std::to_string(difficulty) + "\n"))
		{
			const std::string errorMsg{ "Unable to set Skill Level value to " + std::to_string(difficulty) + "." };
			LOG_WARN(errorMsg);
			consolePanel.AddLog(errorMsg);
			return;
		}

		const std::string message{ "Skill Level value set to " + std::to_string(difficulty) + "." };
		LOG_INFO(message);
		consolePanel.AddLog(message);
	}

	// Overrides skill level
	void ChessEngine::SetDifficultyELO(int elo)
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		if (!WriteToEngine("setoption name UCI_Elo value " + std::to_string(elo) + "\n"))
		{
			const std::string errorMsg{ "Unable to set ELO Rating to " + std::to_string(elo) + "." };
			LOG_WARN(errorMsg);
			consolePanel.AddLog(errorMsg);
			return;
		}

		const std::string message = { "ELO Rating set to " + std::to_string(elo) + "." };
		LOG_INFO(message);
		consolePanel.AddLog(message);
	}

	void ChessEngine::ToggleELO(bool boolean)
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		const std::string toggleMsg = (boolean) ? "true\n" : "false\n";
		if (!WriteToEngine("setoption name UCI_LimitStrength value " + toggleMsg))
		{
			const std::string boolMsg = (boolean) ? "activate" : "deactivate";
			const std::string errorMsg{ "Unable to " + boolMsg + " ELO Rating." };
			LOG_WARN(errorMsg);
			consolePanel.AddLog(errorMsg);
			return;
		}

		const std::string boolMsg = (boolean) ? "activated." : "deactivated.";
		const std::string message = { "ELO Rating " + boolMsg };
		LOG_INFO(message);
		consolePanel.AddLog(message);
	}

	std::string ChessEngine::GetNextMove(const std::string& moveHistory)
	{
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		// Send position to engine
		if (!WriteToEngine("position startpos moves " + moveHistory + "\ngo depth 5\nd\n"))
		{
			const std::string errorMsg{ "Unable to get chess engine's next move." };
			LOG_WARN(errorMsg);
			consolePanel.AddLog(errorMsg);
			return "error";
		}
		
		// Read engine's reply
		std::string engineMessage = ReadFromEngine();
		LOG_INFO(engineMessage);

		// Grab the engine's move
		size_t found = engineMessage.find("bestmove");
		if (found != std::string::npos)
		{
			// Erase everything up to and including "bestmove"
			engineMessage.erase(0, found + sizeof("bestmove"));
			std::istringstream iss{ engineMessage };
			engineMessage.clear();
			iss >> engineMessage; // grab the move

			return engineMessage;
		}
		
		return "error"; // If no bestmove is found
	}

	// Runs a Python script
	std::vector<std::string> ChessEngine::GetValidMoves(const std::string& fen)
	{
		const std::string pythonScriptPath{ "assets/script/__init__.exe" };
		const std::string argument = { pythonScriptPath + " " + fen };

		// Convert std::string into LPSTR
		LPSTR lpPath = const_cast<char*>(pythonScriptPath.c_str());
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
		ConsolePanel& consolePanel = GameLayer::Get().GetConsolePanel();

		// Send position to engine
		if (!WriteToEngine("position startpos moves " + moveHistory + "\nd\n"))
		{
			const std::string errorMsg{ "Unable to get FEN from engine." };
			LOG_ERROR(errorMsg);
			consolePanel.AddLog(errorMsg);
			return "error";
		}

		// Read engine's reply and print it
		std::string engineMessage = ReadFromEngine();
		LOG_INFO(engineMessage);
		consolePanel.AddLog(engineMessage);
		engineMessage.pop_back();

		// Grab the FEN string
		size_t found = engineMessage.find("Fen:");
		if (found != std::string::npos)
		{
			// Erase everything up to and including "Fen:"
			engineMessage.erase(0, found + sizeof("Fen:"));

			// Grab everything until "Key:" is reached
			std::istringstream iss{ engineMessage };
			engineMessage.clear();
			for (std::string str; iss >> str; )
			{
				if (str == "Key:")
				{
					engineMessage.pop_back(); // delete last whitespace
					return engineMessage;	  // only the FEN notation is left, return it
				}
				engineMessage += str + ' ';
			}
		}

		return "error"; // If no FEN string is found
	}

	bool ChessEngine::WriteToEngine(const std::string& data)
	{
		size_t bytesToWrite = data.size();
		DWORD bytesWritten{ 0 };

		while (bytesToWrite > 0)
		{
			BOOL success = WriteFile(
				m_WritePipe_IN,		// The engine's writing pipe handle
				data.c_str(),		// Data to write
				bytesToWrite,		// Amount of bytes to write
				&bytesWritten,		// Amount of bytes that were written
				NULL				// No overlapped structure
			);
			if (!success)
			{
				LOG_ERROR("Unable to write \"{0}\" to chess engine.", data);
				return false;
			}

			bytesToWrite -= bytesWritten;
		}

		return true;
	}

	const std::string ChessEngine::ReadFromEngine()
	{
		Sleep(200);

		std::array<char, 2048> buffer = {};
		std::string msg{};
		DWORD bytesRead{ 0 };

		// Read engine's reply until there are no more bytes
		do
		{
			buffer = {};
			BOOL success = ReadFile(
				m_ReadPipe_OUT,		// The engine's reading pipe handle
				buffer.data(),		// The buffer to store the data read
				buffer.size(),		// Amount of bytes to read
				&bytesRead,			// Amount of bytes that were read
				NULL				// No overlapped structure
			);
			if (!success || bytesRead == 0) break;

			msg += buffer.data();

		} while (bytesRead >= sizeof(buffer));

		return msg;
	}
}

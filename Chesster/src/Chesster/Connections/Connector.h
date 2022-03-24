#pragma once

// Uses Windows types because "When in Rome..."

namespace Chesster
{
	// The maximum size of the buffer
	constexpr DWORD BUFSIZE{ 2048 };

	/// <summary>
	/// 
	/// </summary>
	class Connector
	{
	public:
		Connector();
		~Connector();

		void ConnectToEngine(const LPWSTR& path);

		void ResetGame();
		void EvaluateGame();

		void SetDifficultyLevel(int difficulty = 0);
		void SetDifficultyELO(int elo = 1350); // Overrides skill level
		void ToggleELO(bool boolean);

		std::string GetNextMove(const std::string& position);
		std::vector<std::string> GetValidMoves(const std::string& path, const std::string& fen);
		std::string GetFEN(const std::string& latestMove);

	private:
		void CloseAllConnections();
		const std::string GetEngineReply();

	private:
		// Chess engine
		STARTUPINFO m_StartInfo;
		SECURITY_ATTRIBUTES m_SecAttr;
		PROCESS_INFORMATION m_ProcessInfo;
		HANDLE m_Pipe_IN_Rd, m_Pipe_IN_Wr, m_Pipe_OUT_Rd, m_Pipe_OUT_Wr;
		BYTE m_Buffer[BUFSIZE];
		DWORD m_Read, m_Written;
		BOOL m_Success;

		// Python script
		STARTUPINFOA m_StartInfo_Py;
		PROCESS_INFORMATION m_ProcessInfo_Py;
		BOOL m_Success_Py;
	};
}

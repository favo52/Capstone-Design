#pragma once

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

		void ResetGame();
		void EvaluateGame();

		void SetDifficulty(const int& difficulty);

		std::string GetNextMove(const std::string& position);
		std::vector<std::string> GetValidMoves(const std::string& path, const std::string& fen);
		std::string GetFEN(const std::string& latestMove);

	private:
		const std::string GetEngineReply();
		void CloseConnections();

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

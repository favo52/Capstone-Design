#pragma once

// Uses Microsoft types because "When in Rome..."

namespace Chesster
{	
	constexpr DWORD BUFSIZE{ 2048 };	// The maximum size of the buffers

	/*	Establishes the interprocess communication between the Chesster application, 
		the Stockfish chess engine and the python script used to evaluate all valid moves. */
	class Interprocess
	{
	public:
		/*	Default constructor. Prepares all the resources to establish communication. */
		Interprocess();

		/*	Destructor. Closes all connections. */
		virtual ~Interprocess();

		/* Connects to the UCI chess engine file in the specified path.
		 @param path The filepath of the chess engine executable. */
		void ConnectToEngine(const LPWSTR& path);

		/*	Sends a command to the UCI chess engine to reset the game position to a starting position. */
		void ResetGame();

		/*	Sends a command to the UCI chess engine to evaluate the current standing of the chess game. */
		void EvaluateGame();

		/*	Sends a command to the UCI chess engine to change the skill level difficulty of the engine.
			Defaults to 0. Can be changed in real-time.
		 @param difficulty The new skill level of the engine. */
		void SetDifficultyLevel(int difficulty = 0);

		/*	Sends a command to the UCI chess engine to change the ELO rating of the engine.
			This overrides the skill level. Can be changed in real-time.
		 @param elo The new ELO rating of the engine. */
		void SetDifficultyELO(int elo = 1350); // Overrides skill level

		/*	Overrides the skill level for the ELO rating.
		 @param boolean Toggles ELO if true, toggles skill level if false. */
		void ToggleELO(bool boolean);

		/*	Retrieves the next move of the chess engine.
		 @param moveHistory The current move history of the chess game.
		 @return A std::string of the chess engine's next move. Usually a 4 char string or
				 a 5 char string if there is a pawn promotion. */
		std::string GetNextMove(const std::string& moveHistory);

		/*	Executes the python script to acquire all the valid moves in the current position of the board.
		 @param path The filepath of the python script.
		 @param fen The FEN notation of the current board position.
		 @return A list of all the valid moves in the current positon separated by whitespace. */
		std::vector<std::string> GetValidMoves(const std::string& path, const std::string& fen);

		/*	Retrieves the current FEN notation from the chess engine of the given move history.
		 @param moveHistory The current move history of the board.
		 @return A string of the FEN notation from the given move history. */
		std::string GetFEN(const std::string& moveHistory);

	private:
		/*	Sends the "quit" command to the engine and closes all the interprocess pipes. */
		void CloseAllConnections();

		/*	Retrieves the message from the chess engine after it was given a command.
		 @return A string of the chess engine's reply to the given command. */
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

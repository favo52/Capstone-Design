/*
* Copyright 2022-present, Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#pragma once

// Uses Microsoft types because "When in Rome..."

namespace Chesster
{
	/*	Establishes the interprocess communication between the Chesster application, 
		the Stockfish chess engine and the python script used to evaluate all valid moves. */
	class ChessEngine
	{
	public:
		/*	Creates Pipes and prepares all the resources to create the child processes. */
		ChessEngine();

		/*	Sends the "quit" command to the engine and closes all the interprocess pipes. */
		virtual ~ChessEngine();

		/* Connects to the UCI chess engine file in the specified path.
		 @param path The filepath of the chess engine executable. */
		void ConnectToEngine(const LPWSTR& path);

		/*	Sends a command to the UCI chess engine to reset the game position to a starting position. */
		void NewGame();

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

		/*	Sends a command to the UCI chess engine to change the MultiPV of the engine. 
			The higher the MultiPV, the higher chance for a bad move.
		 @param elo The new ELO rating of the engine. */
		void SetMultiPV(int multipv = 1);

		/*	Overrides the skill level for the ELO rating.
		 @param boolean Toggles ELO if true, toggles skill level if false. */
		void ToggleELO(bool boolean);

		/*	Retrieves the next move of the chess engine.
		 @param moveHistory The current move history of the chess game.
		 @return A string of the chess engine's next move, usually a 4 char string or
				 a 5 char string if there is a pawn promotion. This function
				 returns "error" if it was unable to get the engine's move. */
		std::string GetNextMove(const std::string& moveHistory);

		/*	Retrieves the current FEN notation from the chess engine with the given move history.
		 @param moveHistory The move history of the current game.
		 @return A string of the FEN notation from the given move history. */
		std::string GetFEN(const std::string& moveHistory);

		/*	Executes the python script to acquire all the valid moves in the current position of the board.
		 @param path The filepath of the python script.
		 @param fen The FEN notation of the current board position.
		 @return A list of all the valid moves in the current positon separated by whitespace. */
		std::vector<std::string> GetValidMoves(const std::string& fen);

		/* Used to check if the game ended in stalemate.
		 @param fen The FEN notation of the current board position.
		 @return True if game ended in stalemate, false otherwise. */
		bool IsStalemate(const std::string& fen);

		/**	Sends the data to the chess engine.
		 @param data The data to send, usually an UCI chess engine command.
		 @return True if the data was sent successfully, false if not. */
		bool WriteToEngine(const std::string& data);

		/*	Retrieves the message from the chess engine after it was given a command.
		 @return A string of the chess engine's reply to the given command. */
		const std::string ReadFromEngine();

	private:
		STARTUPINFO m_StartInfo;
		SECURITY_ATTRIBUTES m_SecAttr;
		PROCESS_INFORMATION m_ProcessInfo;
		HANDLE m_ReadPipe_IN, m_WritePipe_IN;
		HANDLE m_ReadPipe_OUT, m_WritePipe_OUT;
	};
}

/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
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

#include "Chesster/Layers/Layer.h"

#include "Chesster/Renderer/Framebuffer.h"

#include "Chesster/Game/Board.h"

#include "Chesster/Connections/ChessEngine.h"
#include "Chesster/Connections/Network.h"

#include "Chesster/Panels/ConsolePanel.h"
#include "Chesster/Panels/LogPanel.h"
#include "Chesster/Panels/SettingsPanel.h"

namespace Chesster
{
	enum class Code;

	class GameLayer : public Layer
	{
	public:
		enum class GameState { Gameplay, Checkmate, Stalemate, PawnPromotion };

		enum class Player { Black = 1, White };
		friend Player operator++(Player& player);

		/* Loads resources, constructs the Board/Pieces. */
		GameLayer();

		/* Starts the Chess Engine thread when Layer is pushed. */
		virtual void OnAttach() override;
		
		/* Stops the Chess Engine thread when Layer is popped. */
		virtual void OnDetach() override;

		/* Handles keyboard/mouse events specific to this Layer.
		 @param sdlEvent The SDL_Event. */
		virtual void OnEvent(SDL_Event& sdlEvent) override;
		
		/* Handles window resizes, blinking text, mouse dragging, and
		   performs the corresponding action when a picture has been taken.
		 @param dt The frametime taken from the Run() function. */
		virtual void OnUpdate(const Timestep& dt) override;
		
		/* Draws the Board, gameover screen and texts to the Viewport window. */
		virtual void OnRender() override;
		
		/* Draws all the ImGui elements to the Viewport window. */
		virtual void OnImGuiRender() override;

		/* Resets the state of the Gamelayer to how it initializes when created.
		   This resets the board, pieces, movehistory, etc. */
		void ResetGame();
		
		/* Sets the OldCameraData equal to START_DATA, the starting board position. */
		void ResetOldCameraData();

		/* Sets m_IsCameraDataReceived to true. */
		void CameraDataReceived() { m_IsCameraDataReceived = true; }

		/* Updates the m_RobotCodes char array to the specified value.
		 @param code The array's index. It's based out of the Code enum class.
		 @param value The new value of the char array. */
		void UpdateRobotCode(Code code, char value);
		
		/* Sets the m_IsNewGameButtonPressed to true. */
		void NewGameButtonPressed() { m_IsNewGameButtonPressed = true; }
		
		/* Sets the m_IsEndPlayerTurn to true. */
		void EndPlayerTurn() { m_IsEndPlayerTurn = true; }
		
		/* Sets the m_IsArmSettled to true. */
		void ArmIsSettled() { m_IsArmSettled = true; }

		/* @return True is the move history is empty, false otherwise. */
		bool IsStartingPosition() const { return m_MoveHistory.empty(); }

		/* @param active The true or false value to set the m_IsEventsActive variable. */
		void SetEventsActive(bool active) { m_IsEventsActive = active; }

		/* @return A reference to the current game state object. */
		GameState& GetGameState() { return m_CurrentGameState; }
		
		/* @return A reference to the current player object. */
		Player& GetCurrentPlayer() { return m_CurrentPlayer; }
		
		/* @return A reference to the Board object. */
		Board& GetBoard() { return m_Board; }

		/* @return A reference to the ChessEngine object. */
		ChessEngine& GetChessEngine() { return m_ChessEngine; }
		
		/* @return A reference to the Network object. */
		Network& GetNetwork() { return *m_Network; }

		/* @return A reference to the ConsolePanel object. */
		ConsolePanel& GetConsolePanel() { return m_ConsolePanel; }
		
		/* @return A reference to the LogPanel object. */
		LogPanel& GetLogPanel() { return m_LogPanel; }
		
		/* @return A reference to the SettingsPanel object. */
		SettingsPanel& GetSettingsPanel() { return m_SettingsPanel; }

		/* It is used to retrieve the instance of the current GameLayer.
		 @return A reference to this GameLayer object. */
		static GameLayer& Get() { return *s_Instance; }

	private:
		/* Performs the computer's move. */
		void UpdateComputerMove();
		
		/* Performs the human player's move based on the picture data. */
		void UpdatePlayerCameraMove();
		
		/* Performs the human player's move based on mouse drag and drop events. */
		void UpdatePlayerMouseMove();

		/* Comapares the previous picture data with the current picture data to
		   find the difference between the two pictures and form the human's move.
		 @return The player's move done in the real world, in algebraic notation. */
		std::string GetCameraMove();

		/*
		 @param point The(x, y) coordinates of the Mouse.
		 @param rectBounds The bounds to check if the point is inside. */
		bool IsPointInRect(const glm::vec2& point, const RectBounds& rectBounds);
		
		/* Searches the list of legal moves to check if the notation is legal in this turn.
		 @param notation The algebraic notation of the current move.
		 @return True if the given notation is a legal move, false otherwise. */
		bool IsMoveLegal(const std::string& notation);

		/* Performs the corresponding actions when the New Game push button is physically pressed. */
		void OnNewGameButtonPressed();

		/* Opens an ImGui window with buttons to select the desired Piece for pawn promotion. 
		   This popup only opens with player mouse moves. */
		void PawnPromotionPopupWindow();
		
		/* Draws the corresponding winner depending on player, or stalemate. */
		void DrawGameoverScreen();

		/* Draws the blinking Illegal Move text. */
		void DrawIllegalMoveText();

		/* Separate thread that performs ChessEngine methods.
		   It stays alive as long as the Gamelayer is active. */
		static void ChessEngineThread();

	private:
		ChessEngine m_ChessEngine;
		
		Framebuffer m_Framebuffer;
		glm::vec2 m_ViewportSize;

		Board m_Board;					// Holds all 64 squares of the board and all 32 chess pieces

		glm::vec2 m_WindowMousePos;		// The mouse position relative to the window
		glm::vec2 m_ViewportMousePos;	// The mouse position within the viewport window
		Piece* m_ClickedPiece;			// A pointer to the current piece clicked by the mouse

		// Moves/Notations
		std::string m_CurrentMove;
		std::string m_MoveHistory;
		std::vector<std::string> m_LegalMoves;

		bool m_IsCameraDataReceived{ false };
		std::vector<std::string> m_NewCameraData;
		std::vector<std::string> m_OldCameraData;
		
		std::array<char, 11> m_RobotCodes;
		bool m_IsNewGameButtonPressed{ false };
		bool m_IsEndPlayerTurn{ false };
		bool m_IsArmSettled{ false };

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Gameplay };

		bool m_IsEventsActive{ false };
		bool m_IsIllegalMove{ false };
		
		std::unique_ptr<Network> m_Network;

		// ImGui Panels
		ConsolePanel m_ConsolePanel;
		LogPanel m_LogPanel;
		SettingsPanel m_SettingsPanel;

		std::shared_ptr<Font> m_AbsEmpireFont;
		std::unique_ptr<Texture> m_GameoverText;
		std::unique_ptr<Texture> m_IllegalMoveText;

	private:
		static GameLayer* s_Instance;	// Pointer to this
	};

	enum class Code
	{
		GameActive,	// Code for playing. 0 = lock, 1 = playing, 2 = illegal move, 3 = white won, 4 = black won, 5 = stalemate
		InitCol,	// First letter of initial position in ASCII code.
		InitRow,	// First number of initial position in ASCII code.
		EndCol,		// First letter of ending position in ASCII code.
		EndRow,		// First number of ending position in ASCII code.
		Special,	// Special movement. 0 = None. 1 = En Passant, 2 = Castling.
		SpecialCol,	// Letter of position for special movement, be it rook to grab for castling or pawn to discard for en passant.
		SpecialRow,	// Number of position for special movement, be it rook to grab for castling or pawn to discard for en passant.
		Capture,	// Capture Code. 0 = false (no box), 1 = true (box).
		Move,		// Move Code. 1 = move, 2 = promote
		Promote		// Promote Code. 2 = Knight, 4 = Queen.
	};

	// Responses to the robot arm
	constexpr char* GAME_LOCKED { "00000000000" };
	constexpr char* GAME_ACTIVE { "10000000000" };
	constexpr char* ILLEGAL_MOVE{ "20000000000" };
	constexpr char* WHITE_WON	{ "30000000000" };
	constexpr char* BLACK_WON	{ "40000000000" };
	constexpr char* STALEMATE	{ "50000000000" };
}

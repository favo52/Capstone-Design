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

#include "Chesster/Core/Layer.h"

#include "Chesster/Renderer/Framebuffer.h"

#include "Chesster/Game/Board.h"

#include "Chesster/Connections/ChessEngine.h"
#include "Chesster/Connections/Network.h"

#include "Chesster/ImGui/ConsolePanel.h"
#include "Chesster/ImGui/LogPanel.h"
#include "Chesster/ImGui/SettingsPanel.h"

namespace Chesster
{
	enum class Code;

	class GameLayer : public Layer
	{
	public:
		enum class GameState { Gameplay, Checkmate, Stalemate, PawnPromotion };

		enum class Player { Black = 1, White };
		friend Player operator++(Player& player);

		GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event& sdlEvent) override;
		virtual void OnUpdate(const Timestep& dt) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

		void ResetGame();

		void CameraDataReceived() { m_CameraDataReceived = true; }

		void UpdateRobotCode(Code code, char value);
		void EndPlayerTurn() { m_IsEndPlayerTurn = true; }
		void ArmIsSettled() { m_IsArmSettled = true; }

		bool IsStartingPosition() const { return m_MoveHistory.empty(); }
		//void ComputerGo();

		void SetEventsActive(bool active) { m_IsEventsActive = active; }

		GameState& GetGameState() { return m_CurrentGameState; }
		Board& GetBoard() { return m_Board; }

		ChessEngine& GetChessEngine() { return m_ChessEngine; }
		Network& GetNetwork() { return *m_Network; }

		ConsolePanel& GetConsolePanel() { return m_ConsolePanel; }
		LogPanel& GetLogPanel() { return m_LogPanel; }
		SettingsPanel& GetSettingsPanel() { return m_SettingsPanel; }

		/** It is used to retrieve the instance of the current GameLayer.
		 @return A reference to this GameLayer object. */
		static GameLayer& Get() { return *s_Instance; }

	private:
		void UpdateComputerMove();
		void UpdatePlayerCameraMove();
		void UpdatePlayerMouseMove();

		std::string GetCameraMove();

		bool IsPointInRect(const glm::vec2& point, const RectBounds& rectBounds);
		bool IsMoveLegal(const std::string& notation);

		void PawnPromotionPopupWindow();	// This popup only opens with player mouse moves
		void GameoverScreen();

		static void ChessEngineThread();	// Multithread

	private:
		ChessEngine m_ChessEngine;
		
		Framebuffer m_Framebuffer;
		glm::vec2 m_ViewportSize;

		Board m_Board;					// Holds all 64 squares of the board and all 32 chess pieces

		glm::vec2 m_MousePos;			// The mouse position relative to the window
		glm::vec2 m_ViewportMousePos;	// The mouse position within the viewport window
		Piece* m_MousePiece;			// A pointer to the current piece clicked by the mouse

		// Moves/Notations
		std::string m_CurrentMove;
		std::string m_MoveHistory;
		std::vector<std::string> m_LegalMoves;

		bool m_CameraDataReceived{ false };
		std::vector<std::string> m_NewCameraData;
		std::vector<std::string> m_OldCameraData;

		std::array<char, 11> m_RobotCodes;
		bool m_IsEndPlayerTurn{ false };
		bool m_IsArmSettled{ false };

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Gameplay };

		bool m_IsEventsActive{ false };
		
		std::unique_ptr<Network> m_Network;

		// ImGui Panels
		ConsolePanel m_ConsolePanel;
		LogPanel m_LogPanel;
		SettingsPanel m_SettingsPanel;

		std::shared_ptr<Font> m_AbsEmpireFont;
		std::unique_ptr<Texture> m_GameoverTextTexture;

	private:
		static GameLayer* s_Instance;	// Pointer to this
	};

	enum class Code
	{
		GameActive,	// Code for playing. 0 = lock, 1 = playing.
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
}

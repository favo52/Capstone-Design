#pragma once

#include "Chesster/Core/Layer.h"

#include "Chesster/Renderer/Framebuffer.h"

#include "Chesster/Game/Board.h"

#include "Chesster/Connections/ChessEngine.h"
#include "Chesster/Connections/Network.h"
#include "Chesster/ImGui/Panels/ConsolePanel.h"
#include "Chesster/ImGui/Panels/SettingsPanel.h"

namespace Chesster
{
	enum class Code;

	class GameLayer : public Layer
	{
	public:
		enum class GameState { Gameplay, Gameover, PawnPromotion };

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

		void SetEventsActive(bool active) { m_IsEventsActive = active; }

		GameState& GetGameState() { return m_CurrentGameState; }
		Board& GetBoard() { return m_Board; }

		ChessEngine& GetChessEngine() { return m_ChessEngine; }
		Network& GetNetwork() { return *m_Network; }
		ConsolePanel& GetConsolePanel() { return m_ConsolePanel; }
		SettingsPanel& GetSettingsPanel() { return m_SettingsPanel; }

		/** It is used to retrieve the instance of the current GameLayer.
		 @return A reference to this GameLayer object. */
		static GameLayer& Get() { return *s_Instance; }

	private:
		void UpdateComputerMove();
		void UpdatePlayerCameraMove();
		void UpdatePlayerMouseMove();
		void UpdatePlayerPawnPromotion();

		std::string GetCameraMove();

		bool IsPointInRect(const glm::vec2& point, const RectBounds& rectBounds);
		bool IsMoveLegal(const std::string& notation);

		void GameoverPopup();
		void PawnPromotionPopup();

		static void ChessEngineThread();	// Multithread

	private:
		enum class Player { Black, White };
		friend Player operator++(Player& player);		

	private:
		ChessEngine m_ChessEngine;
		
		Framebuffer m_Framebuffer;
		glm::vec2 m_ViewportSize;

		Board m_Board;					// Holds all 64 squares of the board

		glm::vec2 m_MousePos;			// The mouse position relative to the window
		glm::vec2 m_ViewportMousePos;	// The mouse position within the viewport window

		// Moves/Notations
		std::string m_CurrentMove;
		std::string m_MoveHistory;
		size_t m_MoveHistorySize{ 0 };

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
		ConsolePanel m_ConsolePanel;
		SettingsPanel m_SettingsPanel;

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

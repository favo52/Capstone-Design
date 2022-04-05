#pragma once

#include "Chesster/Core/Layer.h"
#include "Chesster/Core/Window.h"

#include "Chesster/Connections/Interprocess.h"
#include "Chesster/Connections/Network.h"

#include "Chesster/Renderer/Framebuffer.h"

#include "Chesster/Game/Board.h"

#include "Chesster/ImGui/Panels/ConsolePanel.h"
#include "Chesster/ImGui/Panels/SettingsPanel.h"

namespace Chesster
{
	class GameLayer : public Layer
	{
	public:
		GameLayer();
		virtual ~GameLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event & sdlEvent) override;
		virtual void OnUpdate(const std::chrono::duration<double>&dt) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

		Interprocess* GetConnector() { return &m_Connector; }
		ConsolePanel* GetConsolePanel() { return &s_ConsolePanel; }
		SettingsPanel* GetSettingsPanel() { return &m_SettingsPanel; }
		static Network* GetTCP() { return &s_TCPConnection; }

		/** It is used to retrieve the instance of the current GameLayer.
		 @return A reference to this GameLayer object. */
		static GameLayer& Get() { return *s_Instance; }

	private:
		void UpdateComputerMove();
		void UpdatePlayerMove();
		
		void SetPieceClips();
		void RemovePiece(Piece& piece);
		void ResetPieces();

		void PromotePawn();

		void ResetBoard();
		void EvaluateBoard();

		bool IsPointInRect(const glm::vec2& point, const RectBounds& rectBounds);
		bool IsNotationValid(const std::string& notation);
		bool IsCurrentMoveLegal();

		void GameoverPopup();
		void PawnPromotionPopup();

		// Multithread
		static unsigned int __stdcall EngineThread(void* data);

	private:
		enum class Player { White, Black };
		friend Player operator++(Player& player);

		enum class GameState { Gameplay, Gameover, PawnPromotion };

	private:
		std::unique_ptr<Framebuffer> m_Framebuffer;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		const std::string m_StartPosFEN;
		const std::string m_PathPythonScript;

		// Chess board
		Board m_Board;
		std::unique_ptr<Texture> m_PieceTexture;
		std::array<Piece, 32> m_Pieces;
		uint32_t m_PieceIndex{ 0 };
		Board::Square m_TargetSquare;

		glm::vec2 m_MouseCoords{ 0.0f, 0.0f };
		glm::vec2 m_ViewportMousePos{ 0.0f, 0.0f };

		// Moves/Notations
		std::string m_CurrentMove{ "0000" };
		std::string m_MoveHistory;
		size_t m_MoveHistorySize{ 0 };

		std::string m_CurrentFEN;
		std::vector<std::string> m_LegalMoves;

		std::string m_OldData;
		std::string m_NewData;
		std::string m_TempData;
		std::string m_NewGameData;

		// Boolean switches
		bool m_IsHoldingPiece{ false };
		bool m_IsPieceReleased{ false };
		bool m_IsComputerTurn{ false };
		bool m_IsRecvComputerMove{ false };
		bool m_IsPlayerPlayed{ false };
		bool m_IsMovePlayed{ false };

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Gameplay };

		Interprocess m_Connector;
		static Network s_TCPConnection;

		ConsolePanel s_ConsolePanel;
		SettingsPanel m_SettingsPanel;

	private:
		static GameLayer* s_Instance;	// Pointer to this
	};
}

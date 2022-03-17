#pragma once

#include "Chesster/Core/Layer.h"
#include "Chesster/Core/Window.h"

#include "Chesster/Connections/Connector.h"
#include "Chesster/Connections/TCPConnection.h"

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

	public:
		static ConsolePanel* GetConsolePanel() { return &m_ConsolePanel; }
		static TCPConnection* GetTCP() { return &m_TCPConnection; }

	private:
		void UpdateComputerMove();
		void UpdatePlayerMove();
		
		void RemovePiece(Piece& piece);
		void ResetPieces();

		void PromotePawn();

		void ResetBoard();
		void EvaluateBoard();
		void UpdateDifficulty();

		bool IsPointInQuad(const glm::vec2& point, const QuadBounds& quad);
		bool IsNotationValid(const std::string& notation);
		bool IsCurrentMoveLegal();

		void GameoverPopup();
		void PawnPromotionPopup();

	private:
		// Multithread
		static unsigned int __stdcall EngineThread(void* data);

	private:
		// Rendering
		Window& m_Window;
		std::shared_ptr<Framebuffer> m_Framebuffer;

		bool m_ViewportFocused{ false }, m_ViewportHovered{ false };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		// Chess Engine
		Connector m_Connector{};
		unsigned threadID{ 0 };
		HANDLE hThread{ nullptr };
		const std::string m_PathPythonScript;

		// Chess board
		Board m_Board{};
		std::array<Piece, 32> m_Pieces;
		std::array<SDL_Rect, 32> m_PieceClips{};
		uint32_t m_PieceIndex{ 0 };
		Board::Square m_TargetSquare;

		glm::vec2 m_MouseCoords{ 0.0f, 0.0f };
		glm::vec2 m_ViewportMousePos{ 0.0f, 0.0f };
		uint32_t m_MouseButton{ 0 };

		// Moves/Notations
		std::string m_CurrentMove{ "0000" };
		std::string m_MoveHistory;
		size_t m_MoveHistorySize{ m_MoveHistory.size() };
		const std::string m_StartPosFEN;
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
		bool m_IsOutsideBoard{ false };

		// Panels
		static ConsolePanel m_ConsolePanel;
		static SettingsPanel m_SettingsPanel;

		enum class Player { White, Black };
		friend Player operator++(Player& player);

		enum class GameState { Gameplay, Gameover, PawnPromotion };

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Gameplay };

		static TCPConnection m_TCPConnection;
	};
}
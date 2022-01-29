#pragma once

#include "Chesster_Unleashed/Core/Layer.h"
#include "Chesster_Unleashed/Core/Window.h"
#include "Chesster_Unleashed/Core/Connector.h"

#include "Chesster_Unleashed/Renderer/Framebuffer.h"

#include "Chesster_Unleashed/Game/Board.h"

#include "Chesster_Unleashed/ImGui/Panels/ConsolePanel.h"
#include "Chesster_Unleashed/ImGui/Panels/SettingsPanel.h"

#include <glm/glm.hpp>

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

	private:
		void UpdateComputerMove();
		void UpdatePlayerMove();
		bool UpdateTargetSquare(const std::string& notation);
		
		void RemovePiece(Piece& piece);
		void ResetPieces();

		void ResetBoard();
		void EvaluateBoard();
		void UpdateDifficulty();

		bool IsPointInQuad(const glm::vec2& point, const QuadBounds& quad);
		bool IsCurrentMoveLegal();

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
		std::string m_PathPythonScript;

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
		std::string m_StartPosFEN;
		std::string m_CurrentFEN;
		std::vector<std::string> m_LegalMoves;

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

		enum class Player
		{
			White = 0, Black
		};

		enum class GameState
		{
			Title = 0, Gameplay, Gameover
		};

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Title };
	};
}

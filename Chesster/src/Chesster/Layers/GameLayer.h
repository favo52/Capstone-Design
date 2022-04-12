#pragma once

#include "Chesster/Core/Layer.h"

#include "Chesster/Renderer/Framebuffer.h"

#include "Chesster/Game/Board.h"
#include "Chesster/Game/Piece.h"

#include "Chesster/Connections/Network.h"
#include "Chesster/ImGui/Panels/ConsolePanel.h"
#include "Chesster/ImGui/Panels/SettingsPanel.h"

namespace Chesster
{
	class ChessEngine;

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

		void ResetGame();

		void CameraDataReceived() { m_CameraDataReceived = true; }

		const std::string& GetCurrentMove() const { return m_CurrentMove; }
		std::array<Piece, 32>& GetChessPieces() { return m_ChessPieces; }
		std::array<char, 256>& GetCameraBuffer() { return m_CameraDataBuffer; }

		ChessEngine& GetChessEngine() { return *m_ChessEngine; }
		ConsolePanel* GetConsolePanel() { return &m_ConsolePanel; }
		SettingsPanel* GetSettingsPanel() { return &m_SettingsPanel; }

		/** It is used to retrieve the instance of the current GameLayer.
		 @return A reference to this GameLayer object. */
		static GameLayer& Get() { return *s_Instance; }

	private:
		void UpdateComputerMove();
		void UpdatePlayerCameraMove();
		void UpdatePlayerMouseMove();
		void UpdatePlayerPawnPromotion();
		
		void MovePiece(const std::string& notation);
		void ResetPieces();

		void UpdatePieceCapture();
		void UpdateNewMove();

		bool IsPointInRect(const glm::vec2& point, const RectBounds& rectBounds);
		bool IsMoveLegal(const std::string& notation);

		void GameoverPopup();
		void PawnPromotionPopup();

		// Multithread
		//static unsigned int __stdcall ChessEngineThread(void* data);
		static void ChessEngineThread();

	private:
		enum class Player { White, Black };
		friend Player operator++(Player& player);

		enum class GameState { Gameplay, Gameover, PawnPromotion };

	private:
		std::unique_ptr<ChessEngine> m_ChessEngine;
		//HANDLE m_EngineThread;

		std::unique_ptr<Texture> m_PieceSpriteSheetTexture;
		std::unique_ptr<Framebuffer> m_Framebuffer;
		glm::vec2 m_ViewportSize;

		// Chess board
		std::unique_ptr<Board> m_Board;			// Holds all 64 squares of the board
		std::array<Piece, 32> m_ChessPieces;	// The 32 chess pieces
		Piece* m_CurrentPiece;					// A pointer to the currently held piece

		glm::vec2 m_MousePos;					// The mouse position relative to the window
		glm::vec2 m_ViewportMousePos;			// The mouse position within the viewport window

		// Moves/Notations
		std::string m_CurrentMove;
		std::string m_MoveHistory;
		size_t m_MoveHistorySize{ 0 };

		const std::string m_StartPosFEN;
		std::string m_CurrentFEN;
		std::vector<std::string> m_LegalMoves;

		bool m_CameraDataReceived{ false };
		std::array<char, 256> m_CameraDataBuffer = {};

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Gameplay };
		
		Network m_Network;
		ConsolePanel m_ConsolePanel;
		SettingsPanel m_SettingsPanel;

	private:
		static GameLayer* s_Instance;	// Pointer to this
	};
}

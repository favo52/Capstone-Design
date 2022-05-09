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
	class GameLayer : public Layer
	{
	public:
		GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event& sdlEvent) override;
		virtual void OnUpdate(const std::chrono::duration<double>& dt) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

		void ResetGame();

		void SetCameraDataReceived(bool boolean) { m_CameraDataReceived = boolean; }

		void SetEventsActive(bool active) { m_IsEventsActive = active; }

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

		enum class GameState { Gameplay, Gameover, PawnPromotion };

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

		Player m_CurrentPlayer{ Player::White };
		GameState m_CurrentGameState{ GameState::Gameplay };

		bool m_IsEventsActive{ true };
		
		std::unique_ptr<Network> m_Network;
		ConsolePanel m_ConsolePanel;
		SettingsPanel m_SettingsPanel;

	private:
		static GameLayer* s_Instance;	// Pointer to this
	};
}

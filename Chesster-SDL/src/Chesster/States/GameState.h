#pragma once

#include "State.h"

#include "Chesster/Board.h"
#include "Chesster/ImGuiWindows.h"
#include "Chesster/ClientTCP.h"

namespace Chesster
{
	class GameState : public State
	{
	public:
		GameState(StateStack& stack, Context context);
		virtual ~GameState() = default;

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

		inline static const bool GetWinningColor() { return m_WinningColor; }

	public:
		static AppLogGUI ImGuiMainWindow;
		static AppSettingsGUI ImGuiSettingsWindow;

		static ClientTCP m_ClientTCP;

	private:
		Board m_Board;
		ImGuiWindowFlags m_ImGuiFlags;

		int m_OldDifficulty;
		static bool m_WinningColor;
	};
}

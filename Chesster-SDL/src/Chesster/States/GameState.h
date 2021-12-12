#pragma once

#include "State.h"

#include "Chesster/Board.h"
#include "Chesster/ImGuiWindows.h"

namespace Chesster
{
	class GameState : public State
	{
	public:
		static AppLogGUI ImGuiMainWindow;
		static AppSettingsGUI ImGuiSettingsWindow;

	public:
		GameState(StateStack& stack, Context context);
		virtual ~GameState() = default;

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

		inline static const bool GetWinningColor() { return m_WinningColor; }

	private:
		Board m_Board;
		ImGuiWindowFlags m_ImGuiFlags;

		int m_OldDifficulty;
		static bool m_WinningColor;
	};
}

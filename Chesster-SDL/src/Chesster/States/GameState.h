#pragma once

#include "pch.h"
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

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		Board m_Board;
		ImGuiWindowFlags m_ImGuiFlags;
	};
}

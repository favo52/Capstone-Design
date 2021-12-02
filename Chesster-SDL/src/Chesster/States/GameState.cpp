#include "pch.h"
#include "GameState.h"

namespace Chesster
{
	AppLogGUI GameState::ImGuiMainWindow{};
	AppSettingsGUI GameState::ImGuiSettingsWindow{};

	GameState::GameState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Board{ context.window->get() }
	{
	}

	bool GameState::HandleEvent(SDL_Event& event)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					RequestStackPush(StateID::Pause);

			} break;
		}

		m_Board.HandleEvent(event);

		return true;
	}

	bool GameState::Update(const std::chrono::duration<double>& dt)
	{
		m_Board.Update(dt);

		if (ImGuiSettingsWindow.m_ResetBoard)
		{
			m_Board.ResetBoard();
			ImGuiSettingsWindow.m_ResetBoard = false;
		}

		if (ImGuiSettingsWindow.m_EvaluateBoard)
		{
			m_Board.EvaluateBoard();
			ImGuiSettingsWindow.m_EvaluateBoard = false;
		}

		return true;
	}

	void GameState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 21u, 21u, 255u, 255u);

		m_Board.Draw();

		// Render ImGui
		ImGuiWindowFlags flags =
		{
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus
		};
		//ImGui::ShowDemoWindow();
		ImGuiMainWindow.Draw("Chesster Console", nullptr, flags);
		ImGuiSettingsWindow.Draw("Settings", nullptr, flags);
	}
}

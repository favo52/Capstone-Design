#include "pch.h"
#include "GameState.h"

namespace Chesster
{
	AppLogGUI GameState::ImGuiMainWindow{};
	AppSettingsGUI GameState::ImGuiSettingsWindow{};

	bool GameState::m_WinningColor = 1;

	GameState::GameState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Board{ context.window->get() }
	{
		m_ImGuiFlags =
		{
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus
		};

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.06f, 0.06f, 0.12f, 1.0f };

		//// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		//// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
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

		// Reset button is pressed
		if (ImGuiSettingsWindow.m_ResetBoard)
		{
			m_Board.ResetBoard();
			ImGuiSettingsWindow.m_ResetBoard = false;
		}

		// Evaluate button is pressed
		if (ImGuiSettingsWindow.m_EvaluateBoard)
		{
			m_Board.EvaluateBoard();
			ImGuiSettingsWindow.m_EvaluateBoard = false;
		}

		// Game is over
		if (m_Board.GetValidMoves().empty())
		{
			m_WinningColor = m_Board.GetWinningColor();
			RequestStackPush(StateID::Gameover);
		}

		return true;
	}

	void GameState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 21u, 21u, 255u, 255u);

		m_Board.Draw();

		// Render ImGui
		//ImGui::ShowDemoWindow();
		ImGuiMainWindow.Draw("Chesster Console", nullptr, m_ImGuiFlags);
		ImGuiSettingsWindow.Draw("Settings", nullptr, m_ImGuiFlags);
	}
}

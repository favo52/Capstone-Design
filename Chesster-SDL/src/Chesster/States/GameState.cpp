#include "pch.h"
#include "GameState.h"

namespace Chesster
{
	AppLogGUI GameState::ImGuiMainWindow{};
	AppSettingsGUI GameState::ImGuiSettingsWindow{};

	int AppSettingsGUI::m_Difficulty{ 0 };

	ClientTCP GameState::m_ClientTCP{};

	Board::Player GameState::m_WinningPlayer{ Board::Player::White };

	GameState::GameState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Board{ stack, context },
		m_OldDifficulty{ AppSettingsGUI::m_Difficulty }
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
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.055f, 0.055f, 0.115f, 1.0f };

		// Headers
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

		// Title
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
		if (ImGuiSettingsWindow.m_ResetBoardButton)
		{
			m_Board.ResetBoard();
			ImGuiSettingsWindow.m_ResetBoardButton = false;
		}

		// Evaluate button is pressed
		if (ImGuiSettingsWindow.m_EvaluateBoardButton)
		{
			m_Board.EvaluateBoard();
			ImGuiSettingsWindow.m_EvaluateBoardButton = false;
		}

		// Difficulty is changed
		if (ImGuiSettingsWindow.m_ChangeDifficultyButton ||
			ImGuiSettingsWindow.m_Difficulty != ImGuiSettingsWindow.m_OldDiff)
		{
			m_Board.ChangeDifficulty();
			ImGuiSettingsWindow.m_OldDiff = ImGuiSettingsWindow.m_Difficulty;
			ImGuiSettingsWindow.m_ChangeDifficultyButton = false;
		}

		// Camera button is pressed
		if (ImGuiSettingsWindow.m_ConnectCameraButton)
		{
			if (ImGuiSettingsWindow.m_IsCameraConnecting)
			{
				m_ClientTCP.ConnectCamera();
				m_ClientTCP.SendCommand();
				if (!m_ClientTCP.RecvConfirmation())
					CHESSTER_WARN("Camera did not respond sucessfully.");
			}
			else
				m_ClientTCP.DisconnectCamera();

			ImGuiSettingsWindow.m_ConnectCameraButton = false;
		}

		// Game is over
		if (m_Board.m_Gameover)
		{
			m_WinningPlayer = m_Board.GetWinningPlayer();
			RequestStackPush(StateID::Gameover);
		}

		return true;
	}

	void GameState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 0u, 0u, 0u, 255u);

		m_Board.Draw();

		// Render ImGui
		//ImGui::ShowDemoWindow();
		ImGuiMainWindow.Draw("Chesster Console", nullptr, m_ImGuiFlags);
		ImGuiSettingsWindow.Draw("Settings", nullptr, m_ImGuiFlags);
	}
}

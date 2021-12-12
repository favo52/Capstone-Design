#include "pch.h"
#include "GameOverState.h"

#include "Chesster/States/GameState.h"

namespace Chesster
{
	GameOverState::GameOverState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_GameOverOverlay{ 0u, 0u, static_cast<int>(m_Window->GetWidth()), static_cast<int>(m_Window->GetHeight()) },
		m_MousePos{},
		m_Font{ context.fonts->Get(FontID::Minecraft) },
		m_Font_100{ context.fonts->Get(FontID::OpenSans_100) },
		m_MenuOptions{},
		m_CurrentOption{ GameOverOptions::PlayAgain },
		m_WinningColor{ GameState::GetWinningColor() }
	{
		// Prepare pause text
		SDL_Color White = { 255u, 255u, 255u, 255u };
		m_WinnerText.LoadFromRenderedText(m_Font_100, "THE WINNER IS...", White);
		m_WinnerText.SetPosition
		(
			((m_Window->GetWidth() - m_WinnerText.GetWidth()) / 2) - 280,
			((m_Window->GetHeight() - m_WinnerText.GetHeight()) / 2) - 100
		);

		std::string winner = { "WHITE!" };
		if (m_WinningColor == 1) winner = "BLACK!";
		m_WinningPlayerText.LoadFromRenderedText(m_Font_100, winner, White);
		m_WinningPlayerText.SetPosition
		(
			((m_Window->GetWidth() - m_WinningPlayerText.GetWidth()) / 2) - 280,
			((m_Window->GetHeight() - m_WinningPlayerText.GetHeight()) / 2) - 50
		);

		// Prepare option text
		m_PlayAgainText.LoadFromRenderedText(m_Font, "PLAY AGAIN", White);
		m_MainMenuText.LoadFromRenderedText(m_Font, "MAIN MENU", White);
		m_ExitText.LoadFromRenderedText(m_Font, "EXIT", White);

		m_MenuOptions.push_back(&m_PlayAgainText);
		m_MenuOptions.push_back(&m_MainMenuText);
		m_MenuOptions.push_back(&m_ExitText);

		int y{ 75 };
		for (int i = 0; i < m_MenuOptions.size(); ++i)
		{
			m_MenuOptions[i]->SetPosition
			(
				((m_Window->GetWidth() - m_MenuOptions[i]->GetWidth()) / 2) - 280,
				((m_Window->GetHeight() - m_MenuOptions[i]->GetHeight()) / 2) + y
			);
			y += 50;
		}

		UpdateOptionText();
	}

	bool GameOverState::HandleEvent(SDL_Event& event)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_RETURN:
						SelectOption();
						break;

					case SDLK_UP:
					{
						--m_CurrentOption;
						UpdateOptionText();
					} break;

					case SDLK_DOWN:
					{
						++m_CurrentOption;
						UpdateOptionText();
					} break;
				}
			} break;

			case SDL_FINGERMOTION:
			case SDL_MOUSEMOTION:
			{
				// Get the mouse screen coordinates
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);

				// Get finger touch location in screen coordinates
				m_TouchLocation.x = event.tfinger.x * m_Window->GetWidth();
				m_TouchLocation.y = event.tfinger.y * m_Window->GetHeight();

				// Check if mouse is inside text bounds
				for (int i = 0; i < m_MenuOptions.size(); ++i)
				{
					SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
					if (SDL_PointInRect(&m_MousePos, &textBounds) ||
						SDL_PointInRect(&m_TouchLocation, &textBounds))
					{
						m_CurrentOption = GameOverOptions(i);
						UpdateOptionText();
					}
				}
			} break;

			case SDL_FINGERUP:
			{
				// Get finger touch location in screen coordinates
				m_TouchLocation.x = event.tfinger.x * m_Window->GetWidth();
				m_TouchLocation.y = event.tfinger.y * m_Window->GetHeight();

				for (int i = 0; i < m_MenuOptions.size(); ++i)
				{
					SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
					if (SDL_PointInRect(&m_TouchLocation, &textBounds))
						SelectOption();
				}
			} break;

			case SDL_MOUSEBUTTONUP:
			{
				if (event.button.button == SDL_BUTTON_LEFT)
					for (int i = 0; i < m_MenuOptions.size(); ++i)
					{
						SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
						if (SDL_PointInRect(&m_MousePos, &textBounds))
							SelectOption();
					}
			} break;
		}

		return false;
	}

	bool GameOverState::Update(const std::chrono::duration<double>& dt)
	{
		return false;
	}

	void GameOverState::Draw()
	{
		// Draw a semi transparent square
		SDL_SetRenderDrawColor(Window::Renderer, 0u, 0u, 0u, 200u);
		SDL_RenderFillRect(Window::Renderer, &m_GameOverOverlay);

		m_WinnerText.Draw();
		m_WinningPlayerText.Draw();

		for (const Texture* text : m_MenuOptions)
			text->Draw();
	}

	void GameOverState::UpdateOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		SDL_Color White = { 255u, 255u, 255u };
		SDL_Color Red = { 255u, 0u, 0u };

		// Black all texts
		m_PlayAgainText.LoadFromRenderedText(m_Font, "PLAY AGAIN", White);
		m_MainMenuText.LoadFromRenderedText(m_Font, "MAIN MENU", White);
		m_ExitText.LoadFromRenderedText(m_Font, "EXIT", White);

		// Red the selected text
		switch (m_CurrentOption)
		{
			case GameOverOptions::PlayAgain:
				m_PlayAgainText.LoadFromRenderedText(m_Font, "PLAY AGAIN", Red);
				break;

			case GameOverOptions::MainMenu:
				m_MainMenuText.LoadFromRenderedText(m_Font, "MAIN MENU", Red);
				break;

			case GameOverOptions::Exit:
				m_ExitText.LoadFromRenderedText(m_Font, "EXIT", Red);
				break;
		}
	}

	void GameOverState::SelectOption()
	{
		if (m_CurrentOption == GameOverOptions::PlayAgain)
		{
			RequestStateClear();
			RequestStackPush(StateID::Gameplay);
		}
		else if (m_CurrentOption == GameOverOptions::MainMenu)
		{
			RequestStateClear();
			RequestStackPush(StateID::Menu);
		}
		else if (m_CurrentOption == GameOverOptions::Exit)
		{
			Close();
			RequestStackPop();
			RequestStackPop();
		}
	}

	void GameOverState::Close()
	{
		// Release fonts
		TTF_CloseFont(m_Font.m_Font);
		TTF_CloseFont(m_Font_100.m_Font);

		// Release logo and texts
		m_WinnerText.FreeTexture();
		m_WinningPlayerText.FreeTexture();
		m_PlayAgainText.FreeTexture();
		m_MainMenuText.FreeTexture();
		m_ExitText.FreeTexture();
	}

	GameOverState::GameOverOptions operator++(GameOverState::GameOverOptions& pauseOption)
	{
		return pauseOption = ((pauseOption == GameOverState::GameOverOptions::Exit) ?
			GameOverState::GameOverOptions::PlayAgain :
			GameOverState::GameOverOptions((int)pauseOption + 1));
	}

	GameOverState::GameOverOptions operator--(GameOverState::GameOverOptions& pauseOption)
	{
		return pauseOption = ((pauseOption == GameOverState::GameOverOptions::PlayAgain) ?
			GameOverState::GameOverOptions::Exit :
			GameOverState::GameOverOptions((int)pauseOption - 1));
	}
}

#include "pch.h"
#include "PauseState.h"
#include "GameState.h"

namespace Chesster
{
	PauseState::PauseState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_PauseOverlay{ 0u, 0u, static_cast<int>(m_Window->GetWidth()), static_cast<int>(m_Window->GetHeight()) },
		m_MousePos{},
		m_Font{ context.fonts->Get(FontID::Minecraft) },
		m_PausedText{},
		m_ContinueText{},
		m_MainMenuText{},
		m_ExitText{},
		m_MenuOptions{},
		m_CurrentOption{ PauseOptions::Continue }
	{
		m_fMinecraft100 = context.fonts->Get(FontID::Minecraft_100);

		// Prepare pause text
		SDL_Color White = { 255u, 255u, 255u };
		m_PausedText.LoadFromRenderedText(m_fMinecraft100, "PAUSE", White);
		m_PausedText.SetPosition
		(
			((m_Window->GetWidth() - m_PausedText.GetWidth()) / 2) - 280,
			((m_Window->GetHeight() - m_PausedText.GetHeight()) / 2) - 100
		);
		
		// Prepare option text
		m_ContinueText.LoadFromRenderedText(m_Font, "CONTINUE", White);
		m_MainMenuText.LoadFromRenderedText(m_Font, "MAIN MENU", White);
		m_ExitText.LoadFromRenderedText(m_Font, "EXIT", White);		
		
		m_MenuOptions.push_back(&m_ContinueText);
		m_MenuOptions.push_back(&m_MainMenuText);
		m_MenuOptions.push_back(&m_ExitText);
		
		int y{ 50 };
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

	bool PauseState::HandleEvent(SDL_Event& event)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						RequestStackPop();
						break;

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
						m_CurrentOption = PauseOptions(i);
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

	bool PauseState::Update(const std::chrono::duration<double>& dt)
	{
		return false;
	}

	void PauseState::Draw()
	{
		// Draw a semi transparent square
		SDL_SetRenderDrawColor(Window::Renderer, 0u, 0u, 0u, 200u);
		SDL_RenderFillRect(Window::Renderer, &m_PauseOverlay);

		m_PausedText.Draw();

		for (const Texture* text : m_MenuOptions)
			text->Draw();
	}

	void PauseState::UpdateOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		SDL_Color White = { 255u, 255u, 255u };
		SDL_Color Red = { 255u, 0u, 0u };

		// Black all texts
		m_ContinueText.LoadFromRenderedText(m_Font, "CONTINUE", White);
		m_MainMenuText.LoadFromRenderedText(m_Font, "MAIN MENU", White);
		m_ExitText.LoadFromRenderedText(m_Font, "EXIT", White);

		// Red the selected text
		switch (m_CurrentOption)
		{
			case PauseOptions::Continue:
				m_ContinueText.LoadFromRenderedText(m_Font, "CONTINUE", Red);
				break;

			case PauseOptions::MainMenu:
				m_MainMenuText.LoadFromRenderedText(m_Font, "MAIN MENU", Red);
				break;

			case PauseOptions::Exit:
				m_ExitText.LoadFromRenderedText(m_Font, "EXIT", Red);
				break;
		}
	}

	void PauseState::SelectOption()
	{
		if (m_CurrentOption == PauseOptions::Continue)
		{
			RequestStackPop();
		}
		else if (m_CurrentOption == PauseOptions::MainMenu)
		{
			GameState::ImGuiMainWindow.Clear();
			RequestStateClear();
			RequestStackPush(StateID::Menu);
		}
		else if (m_CurrentOption == PauseOptions::Exit)
		{
			Close();
			RequestStackPop();
			RequestStackPop();
		}
	}

	void PauseState::Close()
	{
		// Release fonts
		TTF_CloseFont(m_fMinecraft100.m_Font);
		TTF_CloseFont(m_Font.m_Font);

		// Release logo and texts
		m_PausedText.FreeTexture();
		m_ContinueText.FreeTexture();
		m_MainMenuText.FreeTexture();
		m_ExitText.FreeTexture();
	}

	PauseState::PauseOptions operator++(PauseState::PauseOptions& pauseOption)
	{
		return pauseOption = ((pauseOption == PauseState::PauseOptions::Exit) ?
			PauseState::PauseOptions::Continue :
			PauseState::PauseOptions((int)pauseOption + 1));
	}

	PauseState::PauseOptions operator--(PauseState::PauseOptions& pauseOption)
	{
		return pauseOption = ((pauseOption == PauseState::PauseOptions::Continue) ?
			PauseState::PauseOptions::Exit :
			PauseState::PauseOptions((int)pauseOption - 1));
	}
}

#include "pch.h"
#include "PauseState.h"

namespace Chesster
{
	PauseState::PauseState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_PauseOverlay{ 0u, 0u, static_cast<int>(m_Window->GetWidth()), static_cast<int>(m_Window->GetHeight()) },
		m_fMinecraft{},
		m_PausedText{},
		m_ContinueText{},
		m_MainMenuText{},
		m_ExitText{},
		m_MenuOptions{},
		m_CurrentOption{ PauseOptions::Continue }
	{
		m_fMinecraft = context.fonts->Get(FontID::Minecraft);
		m_fMinecraft100 = context.fonts->Get(FontID::Minecraft_100);

		// Prepare pause text
		SDL_Color White = { 255u, 255u, 255u };
		m_PausedText.LoadFromRenderedText(m_fMinecraft100, "PAUSE", White);
		m_PausedText.SetPosition((m_Window->GetWidth() - m_PausedText.GetWidth()) / 2,
			((m_Window->GetHeight() - m_PausedText.GetHeight()) / 2) - 150);
		
		// Prepare option text
		m_ContinueText.LoadFromRenderedText(m_fMinecraft, "CONTINUE", White);
		m_MainMenuText.LoadFromRenderedText(m_fMinecraft, "MAIN MENU", White);
		m_ExitText.LoadFromRenderedText(m_fMinecraft, "EXIT", White);		
		
		m_MenuOptions.push_back(&m_ContinueText);
		m_MenuOptions.push_back(&m_MainMenuText);
		m_MenuOptions.push_back(&m_ExitText);
		
		int y{ 120 };
		for (int i = 0; i < m_MenuOptions.size(); ++i)
		{
			m_MenuOptions[i]->SetPosition((m_Window->GetWidth() - m_MenuOptions[i]->GetWidth()) / 2,
				((m_Window->GetHeight() - m_MenuOptions[i]->GetHeight()) / 2) + y);
			y += 50;
		}

		UpdateOptionText();
	}

	void PauseState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 0u, 0u, 0u, 150u);
		SDL_RenderFillRect(Window::Renderer, &m_PauseOverlay);

		m_PausedText.Draw();

		for (const Texture* text : m_MenuOptions)
			text->Draw();
	}

	bool PauseState::Update()
	{
		return false;
	}

	bool PauseState::HandleEvent(const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
			{
				if (m_CurrentOption == PauseOptions::Continue)
				{
					RequestStackPop();
				}
				else if (m_CurrentOption == PauseOptions::MainMenu)
				{
					RequestStateClear();
					RequestStackPush(StateID::Menu);
				}
				else if (m_CurrentOption == PauseOptions::Exit)
				{
					Close();
					RequestStackPop();
					RequestStackPop();
				}
			} break;

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
		}

		return false;
	}

	void PauseState::UpdateOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		SDL_Color White = { 255u, 255u, 255u };
		SDL_Color Red = { 255u, 0u, 0u };

		// Black all texts
		m_ContinueText.LoadFromRenderedText(m_fMinecraft, "CONTINUE", White);
		m_MainMenuText.LoadFromRenderedText(m_fMinecraft, "MAIN MENU", White);
		m_ExitText.LoadFromRenderedText(m_fMinecraft, "EXIT", White);

		// Red the selected text
		switch (m_CurrentOption)
		{
		case PauseOptions::Continue:
			m_ContinueText.LoadFromRenderedText(m_fMinecraft, "CONTINUE", Red);
			break;

		case PauseOptions::MainMenu:
			m_MainMenuText.LoadFromRenderedText(m_fMinecraft, "MAIN MENU", Red);
			break;

		case PauseOptions::Exit:
			m_ExitText.LoadFromRenderedText(m_fMinecraft, "EXIT", Red);
			break;
		}
	}

	void PauseState::Close()
	{
		// Release fonts
		TTF_CloseFont(m_fMinecraft100.m_fMinecraft);
		TTF_CloseFont(m_fMinecraft.m_fMinecraft);

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

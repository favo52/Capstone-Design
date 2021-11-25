#include "pch.h"
#include "MenuState.h"

namespace Chesster
{
	MenuState::MenuState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_LogoTexture{},
		m_MousePos{},
		m_TitleText{},
		m_playText(),
		m_SettingsText{},
		m_ExitText{},
		m_MenuOptions{},
		m_CurrentOption{ MenuOptions::Play }
	{
		m_Font = context.fonts->Get(FontID::Minecraft);
		m_fAbsEmp100 = context.fonts->Get(FontID::AbsEmpire_100);
		m_LogoTexture = &context.textures->Get(TextureID::ChessterLogo);
		
		// Prepare logo
		m_LogoTexture->SetPosition((m_Window->GetWidth() - m_LogoTexture->GetWidth()) / 2,
			((m_Window->GetHeight() - m_LogoTexture->GetHeight()) / 2) - 50);

		// Prepare title text
		SDL_Color Black = { 0u, 0u, 0u };
		m_TitleText.LoadFromRenderedText(m_fAbsEmp100, "CHESSTER", Black);
		m_TitleText.SetPosition((m_Window->GetWidth() - m_TitleText.GetWidth()) / 2,
			((m_Window->GetHeight() - m_TitleText.GetHeight()) / 2) - 250);

		m_playText.LoadFromRenderedText(m_Font, "PLAY", Black);
		m_SettingsText.LoadFromRenderedText(m_Font, "SETTINGS", Black);
		m_ExitText.LoadFromRenderedText(m_Font, "EXIT", Black);

		// Prepare all the menu text
		m_MenuOptions.push_back(&m_playText);
		m_MenuOptions.push_back(&m_SettingsText);
		m_MenuOptions.push_back(&m_ExitText);

		int y{ 180 };
		for (Texture* text : m_MenuOptions)
		{
			text->SetPosition((m_Window->GetWidth() - text->GetWidth()) / 2,
				((m_Window->GetHeight() - text->GetHeight()) / 2) + y);
			y += 50;
		}

		UpdateOptionText();
	}

	void MenuState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 255u, 255u, 255u, 255u);
		
		m_TitleText.Draw();
		m_LogoTexture->Draw();

		for (const Texture* text : m_MenuOptions)
			text->Draw();
	}

	bool MenuState::Update(const std::chrono::duration<double>& dt)
	{
		return true;
	}

	bool MenuState::HandleEvent(const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_RETURN:
				{
					if (m_CurrentOption == MenuOptions::Play)
					{
						RequestStackPop();
						RequestStackPush(StateID::Gameplay);
					}
					else if (m_CurrentOption == MenuOptions::Settings)
					{
						// TODO: Implement a SettingsState
						// SettingsState allows the user to adjust the difficulty level of the engine

						//RequestStackPush(States::SettingsState);
					}
					else if (m_CurrentOption == MenuOptions::Exit)
					{
						Close();

						// By removing itself, the stack will be empty,
						// and the application will know it is time to close.
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

		if (event.type == SDL_MOUSEMOTION)
		{
			// Get the mouse screen coordinates
			SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);

			// Check if mouse is inside text bounds
			for (int i = 0; i < m_MenuOptions.size(); ++i)
			{
				SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
				if (SDL_PointInRect(&m_MousePos, &textBounds))
				{
					m_CurrentOption = MenuOptions(i);
					UpdateOptionText();
				}
			}
		}

		if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
				for (int i = 0; i < m_MenuOptions.size(); ++i)
				{
					SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
					if (SDL_PointInRect(&m_MousePos, &textBounds))
					{
						if (m_CurrentOption == MenuOptions::Play)
						{
							RequestStackPop();
							RequestStackPush(StateID::Gameplay);
						}
						else if (m_CurrentOption == MenuOptions::Settings)
						{
							// SettingsState allows the user to adjust the difficulty level of the engine

							//RequestStackPush(States::SettingsState);
						}
						else if (m_CurrentOption == MenuOptions::Exit)
						{
							Close();
							RequestStackPop();
						}
					}
				}
		}

		return true;
	}

	void MenuState::UpdateOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		SDL_Color Black = { 0u, 0u, 0u };
		SDL_Color Red = { 255u, 0u, 0u };

		// Black all texts
		m_playText.LoadFromRenderedText(m_Font, "PLAY", Black);
		m_SettingsText.LoadFromRenderedText(m_Font, "SETTINGS", Black);
		m_ExitText.LoadFromRenderedText(m_Font, "EXIT", Black);

		// Red the selected text
		switch (m_CurrentOption)
		{
			case MenuOptions::Play:
				m_playText.LoadFromRenderedText(m_Font, "PLAY", Red);
				break;

			case MenuOptions::Settings:
				m_SettingsText.LoadFromRenderedText(m_Font, "SETTINGS", Red);
				break;

			case MenuOptions::Exit:
				m_ExitText.LoadFromRenderedText(m_Font, "EXIT", Red);
				break;
		}
	}

	void MenuState::Close()
	{
		// Release fonts
		TTF_CloseFont(m_fAbsEmp100.m_Font);
		TTF_CloseFont(m_Font.m_Font);

		// Release logo and texts
		m_LogoTexture->FreeTexture();
		m_TitleText.FreeTexture();
		m_playText.FreeTexture();
		m_SettingsText.FreeTexture();
		m_ExitText.FreeTexture();
	}

	MenuState::MenuOptions operator++(MenuState::MenuOptions& menuOption)
	{
		return menuOption = ((menuOption == MenuState::MenuOptions::Exit) ?
			MenuState::MenuOptions::Play :
			MenuState::MenuOptions((int)menuOption + 1));
	}

	MenuState::MenuOptions operator--(MenuState::MenuOptions& menuOption)
	{
		return menuOption = ((menuOption == MenuState::MenuOptions::Play) ?
			MenuState::MenuOptions::Exit :
			MenuState::MenuOptions((int)menuOption - 1));
	}
}

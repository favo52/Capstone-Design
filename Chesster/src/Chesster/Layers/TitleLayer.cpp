#include "pch.h"
#include "Chesster/Layers/TitleLayer.h"

#include "Chesster/Core/Application.h"
#include "Chesster/Renderer/Renderer.h"
#include "Chesster/Renderer/RenderCommand.h"
#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	bool TitleLayer::IsStart{ false };

	TitleLayer::TitleLayer() :
		Layer{ "TitleLayer" },
		m_Window{ Application::Get().GetWindow() }
	{
	}

	void TitleLayer::OnAttach()
	{
		Application& app = Application::Get();

		// Prepare images
		m_GroupNameTexture = &app.Get().m_TextureHolder.Get(TextureID::GroupLogo);
		m_LogoTexture = &app.Get().m_TextureHolder.Get(TextureID::ChessterLogo);

		// Prepare fonts
		m_AbsEmpireFont = app.Get().m_FontHolder.Get(FontID::AbsEmpire);
		m_OpenSansFont = app.Get().m_FontHolder.Get(FontID::OpenSans_Bold);

		// Set up text
		SDL_Color Black = { 0u, 0u, 0u, 255u };
		SDL_Color Red = { 255u, 0u, 0u, 255u };
		m_TitleText.LoadFromRenderedText(m_AbsEmpireFont, "CHESSTER", Black);
		m_StartText.LoadFromRenderedText(m_OpenSansFont, "START", Red);
		m_ExitText.LoadFromRenderedText(m_OpenSansFont, "EXIT", Black);

		m_MenuOptions.push_back(&m_StartText);
		m_MenuOptions.push_back(&m_ExitText);

		OnWindowResize();

		{
			using namespace std::literals;
			m_SplashDuration = 0s;
		}
	}

	void TitleLayer::OnDetach()
	{
		m_GroupNameTexture->FreeTexture();
		m_LogoTexture->FreeTexture();

		m_TitleText.FreeTexture();
		m_StartText.FreeTexture();
		m_ExitText.FreeTexture();
	}

	void TitleLayer::OnEvent(SDL_Event& sdlEvent)
	{
		switch (sdlEvent.type)
		{
			case SDL_WINDOWEVENT:
				if (sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					OnWindowResize();
				break;

			case SDL_KEYDOWN:
			{
				if (sdlEvent.key.keysym.sym == SDLK_RETURN)
					SelectMenuOption();

				if (sdlEvent.key.keysym.sym == SDLK_UP)
				{
					--m_CurrentMenuOption;
					UpdateMenuOptionText();
				}

				if (sdlEvent.key.keysym.sym == SDLK_DOWN)
				{
					++m_CurrentMenuOption;
					UpdateMenuOptionText();
				}

				break;
			}

			case SDL_MOUSEMOTION:
			{
				// Get the mouse screen coordinates
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);

				// Check if mouse or finger is inside text bounds
				for (int i = 0; i < m_MenuOptions.size(); ++i)
				{
					SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
					if (SDL_PointInRect(&m_MousePos, &textBounds))
					{
						m_CurrentMenuOption = MenuOptions(i);
						UpdateMenuOptionText();
					}
				}

				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				{
					for (int i = 0; i < m_MenuOptions.size(); ++i)
					{
						SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
						if (SDL_PointInRect(&m_MousePos, &textBounds))
							SelectMenuOption();
					}
				}

				break;
			}
		}
	}

	void TitleLayer::OnUpdate(const std::chrono::duration<double>& dt)
	{
		m_SplashDuration += dt;

		{
			using namespace std::literals;
			if (m_SplashDuration >= 2s)
				m_CurrentTitleState = TitleState::MainMenu;
		}
	}

	void TitleLayer::OnRender()
	{
		switch (m_CurrentTitleState)
		{
			case TitleState::Splashscreen:
			{
				SDL_Rect background = { 0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };
				Renderer::DrawFilledRect(background, { 255, 255, 255, 255 });
				Renderer::DrawTexture(m_GroupNameTexture);
				break;
			}

			case TitleState::MainMenu:
			{
				SDL_Rect background = { 0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };
				Renderer::DrawFilledRect(background, { 255, 255, 255, 255 });
				Renderer::DrawTexture(m_LogoTexture);
				Renderer::DrawTexture(&m_TitleText);
				Renderer::DrawTexture(&m_StartText);
				Renderer::DrawTexture(&m_ExitText);
				break;
			}
		}
	}

	void TitleLayer::OnWindowResize()
	{
		RepositionTexture(m_GroupNameTexture, 0.0f);
		RepositionTexture(m_LogoTexture, 0.0f);
		RepositionTexture(&m_TitleText, -250.0f);
		RepositionTexture(&m_StartText, 200.0f);
		RepositionTexture(&m_ExitText, 250.0f);
	}

	void TitleLayer::RepositionTexture(Texture* texture, float value)
	{
		int width = Application::Get().GetWindow().GetWidth();
		int height = Application::Get().GetWindow().GetHeight();

		int offsetX = texture->GetWidth() / 2.0f;
		int offsetY = texture->GetHeight() / 2.0f;
		texture->SetPosition((width / 2.0f) - offsetX, ((height / 2.0f) - offsetY) + value);
	}

	void TitleLayer::SelectMenuOption()
	{
		Application& app = Application::Get();

		if (m_CurrentMenuOption == MenuOptions::Start)
		{
			IsStart = true;
		}
		else
		{
			app.Get().Quit();
		}
	}

	void TitleLayer::UpdateMenuOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		SDL_Color Black = { 0u, 0u, 0u, 255u };
		SDL_Color Red = { 255u, 0u, 0u, 255u };

		// Black all texts
		m_StartText.LoadFromRenderedText(m_OpenSansFont, "START", Black);
		m_ExitText.LoadFromRenderedText(m_OpenSansFont, "EXIT", Black);

		// Red the selected text
		switch (m_CurrentMenuOption)
		{
			case MenuOptions::Start:
				m_StartText.LoadFromRenderedText(m_OpenSansFont, "START", Red);
				break;

			case MenuOptions::Exit:
				m_ExitText.LoadFromRenderedText(m_OpenSansFont, "EXIT", Red);
				break;
		}
	}

	TitleLayer::MenuOptions operator++(TitleLayer::MenuOptions& menuOption)
	{
		return menuOption = ((menuOption == TitleLayer::MenuOptions::Exit) ?
			TitleLayer::MenuOptions::Start : TitleLayer::MenuOptions::Exit);
	}

	TitleLayer::MenuOptions operator--(TitleLayer::MenuOptions& menuOption)
	{
		return menuOption = ((menuOption == TitleLayer::MenuOptions::Start) ?
			TitleLayer::MenuOptions::Exit : TitleLayer::MenuOptions::Start);
	}
}

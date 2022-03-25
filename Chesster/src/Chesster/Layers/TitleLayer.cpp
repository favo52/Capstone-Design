#include "pch.h"
#include "Chesster/Layers/TitleLayer.h"

#include "Chesster/Core/Application.h"
#include "Chesster/Renderer/Renderer.h"

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
		// Prepare fonts
		m_AbsEmpireFont = std::make_shared<Font>("assets/fonts/aAbsoluteEmpire.ttf", 100);
		m_OpenSansFont = std::make_shared<Font>("assets/fonts/opensans/OpenSans-Bold.ttf");

		// Prepare images
		m_GroupNameTexture = std::make_unique<Texture>("assets/textures/ReadySetCode.jpeg");
		m_LogoTexture = std::make_unique<Texture>("assets/textures/ChessterLogo.png");

		// Set up text
		SDL_Color Black = { 0u, 0u, 0u, 255u };
		SDL_Color Red = { 255u, 0u, 0u, 255u };
		m_TitleText = std::make_unique<Texture>(m_AbsEmpireFont, "CHESSTER", Black);
		m_StartText = std::make_unique<Texture>(m_OpenSansFont, "START", Red);
		m_ExitText = std::make_unique<Texture>(m_OpenSansFont, "EXIT", Black);

		m_MenuOptionsBounds.push_back(&m_StartText->GetBounds());
		m_MenuOptionsBounds.push_back(&m_ExitText->GetBounds());

		OnWindowResize();

		using namespace std::literals;
		m_SplashDuration = 0s;
	}

	void TitleLayer::OnDetach()
	{
		m_GroupNameTexture->FreeTexture();
		m_LogoTexture->FreeTexture();

		m_TitleText->FreeTexture();
		m_StartText->FreeTexture();
		m_ExitText->FreeTexture();

		m_AbsEmpireFont->CloseFont();
		m_OpenSansFont->CloseFont();
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

				// Check if mouse
				for (int i = 0; i < m_MenuOptionsBounds.size(); ++i)
				{
					if (SDL_PointInRect(&m_MousePos, m_MenuOptionsBounds[i]))
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
					for (int i = 0; i < m_MenuOptionsBounds.size(); ++i)
					{
						if (SDL_PointInRect(&m_MousePos, m_MenuOptionsBounds[i]))
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

		using namespace std::literals;
		if (m_SplashDuration >= 2s) // 2 seconds
			m_CurrentTitleState = TitleState::MainMenu;
	}

	void TitleLayer::OnRender()
	{
		const glm::vec4 clearColor = { 255, 255, 255, 255 };
		Renderer::SetClearColor(clearColor);
		Renderer::Clear();

		switch (m_CurrentTitleState)
		{
			case TitleState::Splashscreen:
			{
				Renderer::DrawTexture(m_GroupNameTexture.get());
				break;
			}

			case TitleState::MainMenu:
			{
				Renderer::DrawTexture(m_LogoTexture.get());
				Renderer::DrawTexture(m_TitleText.get());
				Renderer::DrawTexture(m_StartText.get());
				Renderer::DrawTexture(m_ExitText.get());
				break;
			}
		}
	}

	void TitleLayer::OnWindowResize()
	{
		RepositionTexture(m_GroupNameTexture.get(), 0.0f);
		RepositionTexture(m_LogoTexture.get(), 0.0f);
		RepositionTexture(m_TitleText.get(), -250.0f);
		RepositionTexture(m_StartText.get(), 200.0f);
		RepositionTexture(m_ExitText.get(), 250.0f);
	}

	void TitleLayer::RepositionTexture(Texture* texture, float value)
	{
		int offsetX = texture->GetWidth() / 2.0f;
		int offsetY = texture->GetHeight() / 2.0f;
		texture->SetPosition((m_Window.GetWidth() / 2.0f) - offsetX, ((m_Window.GetHeight() / 2.0f) - offsetY) + value);
	}

	void TitleLayer::SelectMenuOption()
	{
		if (m_CurrentMenuOption == MenuOptions::Start)
			IsStart = true;
		else
			Application::Get().Quit();
	}

	void TitleLayer::UpdateMenuOptionText()
	{
		if (m_MenuOptionsBounds.empty())
			return;

		SDL_Color Black = { 0u, 0u, 0u, 255u };
		SDL_Color Red = { 255u, 0u, 0u, 255u };

		// Black all texts
		m_StartText = std::make_unique<Texture>(m_OpenSansFont, "START", Black);
		m_ExitText = std::make_unique<Texture>(m_OpenSansFont, "EXIT", Black);

		// Red the selected text
		switch (m_CurrentMenuOption)
		{
			case MenuOptions::Start:
				m_StartText = std::make_unique<Texture>(m_OpenSansFont, "START", Red);
				break;
		
			case MenuOptions::Exit:
				m_ExitText = std::make_unique<Texture>(m_OpenSansFont, "EXIT", Red);
				break;
		}

		// Update text rendering status
		m_MenuOptionsBounds[0] = &m_StartText->GetBounds();
		m_MenuOptionsBounds[1] = &m_ExitText->GetBounds();

		RepositionTexture(m_StartText.get(), 200.0f);
		RepositionTexture(m_ExitText.get(), 250.0f);
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

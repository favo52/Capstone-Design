/*
* Copyright 2022-present, Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#include "pch.h"
#include "Chesster/Layers/TitleLayer.h"

#include "Chesster/Core/Application.h"
#include "Chesster/Layers/Gamelayer.h"

namespace Chesster
{
	void TitleLayer::OnAttach()
	{
		// Prepare fonts
		m_AbsEmpireFont = std::make_shared<Font>("assets/fonts/aAbsoluteEmpire.ttf", 100);
		m_OpenSansFont = std::make_shared<Font>("assets/fonts/opensans/OpenSans-Bold.ttf");

		// Prepare images
		m_GroupNameTexture = std::make_unique<Texture>("assets/textures/ReadySetCode.jpeg");
		m_LogoTexture = std::make_unique<Texture>("assets/textures/ChessterLogo.png");

		// Set up text
		const SDL_Color Black = { 0u, 0u, 0u, 255u }, Red = { 255u, 0u, 0u, 255u };
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

			// Menu Keyboard selection
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
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);

				// Check if mouse is hovering menu option
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

	void TitleLayer::OnUpdate(const Timestep& dt)
	{
		m_SplashDuration += dt;

		using namespace std::literals;
		if (m_SplashDuration >= 2s) // 2 seconds
			m_TitleState = TitleState::MainMenu;
	}

	void TitleLayer::OnRender()
	{
		const glm::vec4 clearColor = { 255, 255, 255, 255 };
		Renderer::SetClearColor(clearColor);
		Renderer::Clear();

		switch (m_TitleState)
		{
			case TitleState::Splashscreen:
			{
				Renderer::DrawTexture(m_GroupNameTexture);
				break;
			}

			case TitleState::MainMenu:
			{
				Renderer::DrawTexture(m_LogoTexture);
				Renderer::DrawTexture(m_TitleText);
				Renderer::DrawTexture(m_StartText);
				Renderer::DrawTexture(m_ExitText);
				break;
			}
		}
	}

	void TitleLayer::OnWindowResize()
	{
		RepositionTexture(m_GroupNameTexture, 0.0f);
		RepositionTexture(m_LogoTexture, 0.0f);
		RepositionTexture(m_TitleText, -250.0f);
		RepositionTexture(m_StartText, 200.0f);
		RepositionTexture(m_ExitText, 250.0f);
	}

	void TitleLayer::RepositionTexture(const std::unique_ptr<Texture>& texture, float value)
	{
		Window& window = Application::Get().GetWindow();

		int offsetX = texture->GetWidth() / 2.0f;
		int offsetY = texture->GetHeight() / 2.0f;
		texture->SetPosition((window.GetWidth() / 2.0f) - offsetX, ((window.GetHeight() / 2.0f) - offsetY) + value);
	}

	void TitleLayer::SelectMenuOption()
	{
		Application& app = Application::Get();
		
		if (m_CurrentMenuOption == MenuOptions::Start)
		{
			app.RequestLayerAction(Layer::Action::PopLayer, shared_from_this());
			app.RequestLayerAction(Layer::Action::PushLayer, std::make_shared<GameLayer>());
		}
		else
		{
			app.Quit();
		}
	}

	void TitleLayer::UpdateMenuOptionText()
	{
		if (m_MenuOptionsBounds.empty())
			return;

		const SDL_Color Black = { 0u, 0u, 0u, 255u };
		const SDL_Color Red = { 255u, 0u, 0u, 255u };

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

		RepositionTexture(m_StartText, 200.0f);
		RepositionTexture(m_ExitText, 250.0f);
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

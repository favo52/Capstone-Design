#include "pch.h"
#include "TitleState.h"
#include "Chesster/Core/ResourceHolder.h"

namespace Chesster
{
	TitleState::TitleState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_ReadySetGoTexture{ nullptr },
		m_ChessterLogoTexture{ nullptr },
		m_Font{},
		m_PressKeyText{},
		m_TexEffectTime{},
		m_ShowText{ true },
		m_ShowReadySetCode{ true },
		m_ShowChessterLogo{ false }
	{
		{
			using namespace std::literals;
			m_TexEffectTime = 0s;
		}

		// Prepare text
		m_Font = context.fonts->Get(FontID::OpenSans_100);
		m_PressKeyText.LoadFromRenderedText(m_Font, "CLICK OR PRESS ANY KEY TO CONTINUE", { 0u, 0u, 0u, 255u });
		m_PressKeyText.SetPosition((m_Window->GetWidth() - m_PressKeyText.GetWidth()) / 2,
								  ((m_Window->GetHeight() - m_PressKeyText.GetHeight()) / 2) * 1.70);

		// Prepare logos
		m_ReadySetGoTexture = &context.textures->Get(TextureID::ReadySetCode);
		m_ChessterLogoTexture = &context.textures->Get(TextureID::ChessterLogo);

		m_ReadySetGoTexture->SetPosition((m_Window->GetWidth() - m_ReadySetGoTexture->GetWidth()) / 2, // x
										 ((m_Window->GetHeight() - m_ReadySetGoTexture->GetHeight()) / 2) - 50); // y

		m_ChessterLogoTexture->SetPosition((m_Window->GetWidth() - m_ReadySetGoTexture->GetWidth()) / 2,
										   (m_Window->GetHeight() - m_ReadySetGoTexture->GetHeight()) / 2);
	}

	bool TitleState::HandleEvent(SDL_Event& event)
	{
		// If click or any key is pressed, tigger the next screen
		if (event.type == SDL_KEYDOWN ||
			event.type == SDL_MOUSEBUTTONUP ||
			event.type == SDL_FINGERUP)
		{
			RequestStackPop();
			RequestStackPush(StateID::Menu);
		}

		return true;
	}

	bool TitleState::Update(const std::chrono::duration<double>& dt)
	{
		m_TexEffectTime += dt;

		// Make text blink
		using namespace std::literals;
		if (m_TexEffectTime >= 0.5s)
		{
			m_ShowText = !m_ShowText;
			m_TexEffectTime = 0s;
		}

		return true;
	}

	void TitleState::Draw()
	{
		if (m_ShowText)
			m_PressKeyText.Draw();

		// Render texture to screen
		if (m_ShowReadySetCode)
			m_ReadySetGoTexture->Draw();
		else
			m_ChessterLogoTexture->Draw();
	}
}

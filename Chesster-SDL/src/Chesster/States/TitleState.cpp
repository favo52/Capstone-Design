#include "pch.h"
#include "TitleState.h"
#include "Chesster/ResourceHolder.h"

namespace Chesster
{
	TitleState::TitleState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_ReadySetGoTexture{ nullptr },
		m_ChessterLogoTexture{ nullptr },
		m_Font{},
		m_PressKeyText{},
		m_ShowReadySetCode{ true },
		m_ShowChessterLogo{ false }
	{
		// Prepare text
		m_Font = context.fonts->Get(FontID::Sansation);
		m_PressKeyText.LoadFromRenderedText(m_Font, "PRESS ANY KEY TO CONTINUE", { 0u, 0u, 0u, 255u });
		m_PressKeyText.SetPosition((m_Window->GetWidth() - m_PressKeyText.GetWidth()) / 2,
								  ((m_Window->GetHeight() - m_PressKeyText.GetHeight()) / 2) + 200);

		// Prepare logos
		m_ReadySetGoTexture = &context.textures->Get(TextureID::ReadySetCode);
		m_ChessterLogoTexture = &context.textures->Get(TextureID::ChessterLogo);

		m_ReadySetGoTexture->SetPosition((m_Window->GetWidth() - m_ReadySetGoTexture->GetWidth()) / 2,
										 ((m_Window->GetHeight() - m_ReadySetGoTexture->GetHeight()) / 2) - 50);

		m_ChessterLogoTexture->SetPosition((m_Window->GetWidth() - m_ReadySetGoTexture->GetWidth()) / 2,
										   (m_Window->GetHeight() - m_ReadySetGoTexture->GetHeight()) / 2);
	}

	void TitleState::Draw()
	{
		m_PressKeyText.Draw();

		// Render texture to screen
		if (m_ShowReadySetCode)
			m_ReadySetGoTexture->Draw();
		else
			m_ChessterLogoTexture->Draw();
	}

	bool TitleState::Update()
	{
		return true;
	}

	bool TitleState::HandleEvent(const SDL_Event& event)
	{
		// If any key is pressed, tigger the next screen
		if (event.type == SDL_KEYDOWN)
		{
			//Close();
			RequestStackPop();
			RequestStackPush(StateID::Menu);
		}

		if (event.type == SDL_MOUSEBUTTONUP)
		{
			RequestStackPop();
			RequestStackPush(StateID::Menu);
		}

		return true;
	}

	void TitleState::Close()
	{
		// Release font
		TTF_CloseFont(m_Font.m_Font);

		// Release logo and texts
		m_PressKeyText.FreeTexture();
		m_ReadySetGoTexture->FreeTexture();
		m_ChessterLogoTexture->FreeTexture();
	}
}

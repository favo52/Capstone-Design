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
		m_fMinecraft{},
		m_ShowReadySetCode{ true },
		m_ShowChessterLogo{ false }
	{
		m_fMinecraft = &context.fonts->Get(FontID::Minecraft);

		m_ReadySetGoTexture = &context.textures->Get(TextureID::ReadySetCode);
		m_ChessterLogoTexture = &context.textures->Get(TextureID::ChessterLogo);

		m_ReadySetGoTexture->SetPosition((m_Window->GetWidth() - m_ReadySetGoTexture->GetWidth()) / 2,
										(m_Window->GetHeight() - m_ReadySetGoTexture->GetHeight()) / 2);

		m_ChessterLogoTexture->SetPosition((m_Window->GetWidth() - m_ReadySetGoTexture->GetWidth()) / 2,
											(m_Window->GetHeight() - m_ReadySetGoTexture->GetHeight()) / 2);
	}

	void TitleState::Draw()
	{
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
			RequestStackPop();
			RequestStackPush(StateID::Menu);
		}

		return true;
	}
}

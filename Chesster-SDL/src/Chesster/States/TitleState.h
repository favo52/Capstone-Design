#pragma once

#include "State.h"
#include "Chesster/Texture.h"

namespace Chesster
{
	class TitleState : public State
	{
	public:
		TitleState(StateStack& stack, Context context);

		void Draw() override;
		bool Update() override;
		bool HandleEvent(const SDL_Event& event) override;

	private:
		Window* m_Window;
		Texture* m_ReadySetGoTexture;
		Texture* m_ChessterLogoTexture;

		Texture::Font* m_fMinecraft;

		bool m_ShowReadySetCode;
		bool m_ShowChessterLogo;
	};
}
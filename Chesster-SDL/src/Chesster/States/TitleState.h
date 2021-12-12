#pragma once

#include "State.h"

namespace Chesster
{
	class Texture;

	class TitleState : public State
	{
	public:
		TitleState(StateStack& stack, Context context);

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		Window* m_Window;
		Texture* m_ReadySetGoTexture;
		Texture* m_ChessterLogoTexture;

		// Intro text and blink effect
		Font m_Font;
		Texture m_PressKeyText;
		std::chrono::duration<double> m_TexEffectTime;
		bool m_ShowText;

		bool m_ShowReadySetCode;
		bool m_ShowChessterLogo;
	};
}

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
		bool Update(const std::chrono::duration<double>& dt) override;
		bool HandleEvent(const SDL_Event& event) override;

	private:
		void Close();

	private:
		Window* m_Window;
		Texture* m_ReadySetGoTexture;
		Texture* m_ChessterLogoTexture;

		Texture::Font m_Font;
		Texture m_PressKeyText;
		std::chrono::duration<double> m_TexEffectTime;
		bool m_ShowText;

		bool m_ShowReadySetCode;
		bool m_ShowChessterLogo;
	};
}

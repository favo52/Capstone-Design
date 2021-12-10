#pragma once

#include "State.h"

namespace Chesster
{
	class GameOverState : public State
	{
	public:
		GameOverState(StateStack& stack, Context context);

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		enum class GameOverOptions
		{
			PlayAgain,
			MainMenu,
			Exit
		};

		friend GameOverOptions operator++(GameOverOptions& menuOption);
		friend GameOverOptions operator--(GameOverOptions& menuOption);

		void UpdateOptionText();
		void SelectOption();
		void Close();

	private:
		Window* m_Window;
		SDL_Rect m_GameOverOverlay;

		SDL_Point m_MousePos;

		// Text stuff
		Font m_Font;
		Font m_Font_100;
		Texture m_WinnerText;
		Texture m_WinningPlayerText;
		Texture m_PlayAgainText;
		Texture m_MainMenuText;
		Texture m_ExitText;

		std::vector<Texture*> m_MenuOptions;
		GameOverOptions m_CurrentOption;

		bool m_WinningColor;
	};
}

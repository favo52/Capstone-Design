#pragma once

#include "State.h"

namespace Chesster
{
	class PauseState : public State
	{
	public:
		PauseState(StateStack& stack, Context context);

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		enum class PauseOptions
		{
			Continue,
			MainMenu,
			Exit
		};

		friend PauseOptions operator++(PauseOptions& menuOption);
		friend PauseOptions operator--(PauseOptions& menuOption);
		
		void UpdateOptionText();
		void SelectOption();
		void Close();

	private:
		Window* m_Window;
		SDL_Rect m_PauseOverlay;

		SDL_Point m_MousePos;

		// Text stuff
		Font m_Font;
		Font m_fMinecraft100;
		Texture m_PausedText;
		Texture m_ContinueText;
		Texture m_MainMenuText;
		Texture m_ExitText;
		std::vector<Texture*> m_MenuOptions;
		PauseOptions m_CurrentOption;
	};
}

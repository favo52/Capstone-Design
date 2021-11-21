#pragma once

#include "State.h"

namespace Chesster
{
	class PauseState : public State
	{
	public:
		PauseState(StateStack& stack, Context context);

		void Draw() override;
		bool Update() override;
		bool HandleEvent(const SDL_Event& event) override;
				 
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
		void Close();

	private:
		Window* m_Window;
		SDL_Rect m_PauseOverlay;

		Texture::Font m_fMinecraft;
		Texture::Font m_fMinecraft100;
		Texture m_PausedText;
		Texture m_ContinueText;
		Texture m_MainMenuText;
		Texture m_ExitText;
		std::vector<Texture*> m_MenuOptions;
		PauseOptions m_CurrentOption;
	};
}

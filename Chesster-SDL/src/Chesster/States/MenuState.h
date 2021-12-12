#pragma once

#include "State.h"

namespace Chesster
{
	class MenuState : public State
	{
	public:
		MenuState(StateStack& stack, Context context);

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		enum class MenuOptions
		{
			Play,
			Settings,
			Exit
		};

		friend MenuOptions operator++(MenuOptions& menuOption);
		friend MenuOptions operator--(MenuOptions& menuOption);

		void UpdateOptionText();
		void SelectOption();
		void Close();

	private:
		Window* m_Window;
		Texture* m_LogoTexture;

		SDL_Point m_MousePos;
		SDL_Point m_TouchLocation;

		// Options Text
		Font m_Font;
		Font m_fAbsEmp100;
		Texture m_TitleText;
		Texture m_playText;
		Texture m_SettingsText;
		Texture m_ExitText;
		std::vector<Texture*> m_MenuOptions;
		MenuOptions m_CurrentOption;
	};
}

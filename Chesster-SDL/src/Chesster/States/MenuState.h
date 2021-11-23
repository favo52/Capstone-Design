#pragma once

#include "State.h"

namespace Chesster
{
	class MenuState : public State
	{
	public:
		MenuState(StateStack& stack, Context context);

		void Draw() override;
		bool Update() override;
		bool HandleEvent(const SDL_Event& event) override;

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
		void Close();

	private:
		Window* m_Window;
		Texture* m_LogoTexture;

		SDL_Point m_MousePos;
		//Vector2f m_BoardOffset;

		// Options Text
		Texture::Font m_Font;
		Texture::Font m_fAbsEmp100;
		Texture m_TitleText;
		Texture m_playText;
		Texture m_SettingsText;
		Texture m_ExitText;
		std::vector<Texture*> m_MenuOptions;
		MenuOptions m_CurrentOption;
	};
}

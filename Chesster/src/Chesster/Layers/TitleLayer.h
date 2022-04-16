#pragma once

#include "Chesster/Core/Layer.h"

namespace Chesster
{
	// Forward declarations
	class Window;
	class Texture;
	class Font;

	/*	 */
	class TitleLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event& sdlEvent) override;
		virtual void OnUpdate(const std::chrono::duration<double>& dt) override;
		virtual void OnRender() override;

		static bool s_IsStart;

	private:
		void OnWindowResize();
		void RepositionTexture(Texture* texture, float value);

		void SelectMenuOption();
		void UpdateMenuOptionText();

	private:
		enum class TitleState { Splashscreen, MainMenu };
		enum class MenuOptions { Start, Exit };

	private:
		std::shared_ptr<Font> m_AbsEmpireFont;			// The title's font
		std::shared_ptr<Font> m_OpenSansFont;			// The menu's font

		std::unique_ptr<Texture> m_GroupNameTexture;	// The groups logo image
		std::unique_ptr<Texture> m_LogoTexture;			// Chesster's logo image

		std::unique_ptr<Texture> m_TitleText;
		std::unique_ptr<Texture> m_StartText;
		std::unique_ptr<Texture> m_ExitText;

		SDL_Point m_MousePos{ 0, 0 };
		std::chrono::duration<double> m_SplashDuration{};

		std::vector<const SDL_Rect*> m_MenuOptionsBounds;
		
		TitleState m_TitleState{ TitleState::Splashscreen };

		MenuOptions m_CurrentMenuOption{ MenuOptions::Start };
		friend MenuOptions operator++(MenuOptions& menuOption);
		friend MenuOptions operator--(MenuOptions& menuOption);
	};
}

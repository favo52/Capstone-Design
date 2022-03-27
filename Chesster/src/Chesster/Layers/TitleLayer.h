#pragma once

#include "Chesster/Core/Layer.h"
#include "Chesster/Renderer/Texture.h"

namespace Chesster
{
	class Window;

	class TitleLayer : public Layer
	{
	public:
		TitleLayer();
		virtual ~TitleLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event& sdlEvent) override;
		virtual void OnUpdate(const std::chrono::duration<double>& dt) override;
		virtual void OnRender() override;

	public:
		static bool IsStart;

	private:
		void OnWindowResize();
		void RepositionTexture(Texture* texture, float value);

		void SelectMenuOption();
		void UpdateMenuOptionText();

	private:
		Window& m_Window; // Reference to the current window

		std::shared_ptr<Font> m_AbsEmpireFont;
		std::shared_ptr<Font> m_OpenSansFont;

		std::unique_ptr<Texture> m_GroupNameTexture;
		std::unique_ptr<Texture> m_LogoTexture;

		std::unique_ptr<Texture> m_TitleText;
		std::unique_ptr<Texture> m_StartText;
		std::unique_ptr<Texture> m_ExitText;

		std::vector<const SDL_Rect*> m_MenuOptionsBounds;

		SDL_Point m_MousePos{ 0, 0 };

		std::chrono::duration<double> m_SplashDuration{};

		enum class TitleState { Splashscreen, MainMenu };
		TitleState m_CurrentTitleState{ TitleState::Splashscreen };

		enum class MenuOptions { Start, Exit };
		MenuOptions m_CurrentMenuOption{ MenuOptions::Start };

		friend MenuOptions operator++(MenuOptions& menuOption);
		friend MenuOptions operator--(MenuOptions& menuOption);
	};
}

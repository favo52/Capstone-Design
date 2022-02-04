#pragma once

#include "Chesster_Unleashed/Core/Window.h"
#include "Chesster_Unleashed/Core/Layer.h"
#include "Chesster_Unleashed/Renderer/Texture.h"

namespace Chesster
{
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
		Window& m_Window;
		Texture* m_GroupNameTexture{ nullptr };
		Texture* m_LogoTexture{ nullptr };

		Font m_OpenSansFont;
		Font m_AbsEmpireFont;
		Texture m_TitleText;
		Texture m_StartText;
		Texture m_ExitText;
		std::vector<Texture*> m_MenuOptions;

		SDL_Point m_MousePos{ 0, 0 };

		std::chrono::duration<double> m_SplashDuration{};

		enum class TitleState { Splashscreen, MainMenu };
		enum class MenuOptions { Start, Exit };

		friend MenuOptions operator++(MenuOptions& menuOption);
		friend MenuOptions operator--(MenuOptions& menuOption);

		TitleState m_CurrentTitleState{ TitleState::Splashscreen };
		MenuOptions m_CurrentMenuOption{ MenuOptions::Start };
	};
}

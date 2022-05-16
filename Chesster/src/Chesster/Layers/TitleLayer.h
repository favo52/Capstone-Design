#pragma once

#include "Chesster/Core/Layer.h"

namespace Chesster
{
	// Forward declarations
	class Window;
	class Texture;
	class Font;

	/* The program's starting Layer. Displays the group's logos. */
	class TitleLayer : public Layer, public std::enable_shared_from_this<TitleLayer>
	{
	public:
		/* Prepares and load, the Group's Logo Images and fonts and configure the title text. */
		virtual void OnAttach() override;

		/* Releases all the Textures when the Layer gets popped. */
		virtual void OnDetach() override;

		/** Handles mouse and keyboard input.
		 @param sdlEvent The user's input detected by the SDL library. */
		virtual void OnEvent(SDL_Event& sdlEvent) override;

		/** Changes TitleState from Splashscreen to MainMenu after 2 seconds.
		 @param dt The delta time taken from Run(). */
		virtual void OnUpdate(const Timestep& dt) override;

		/* Draws all the Textures. */
		virtual void OnRender() override;

	private:
		/* Repositions all the Textures when the user resizes the Window. */
		void OnWindowResize();

		/** Repositions the texture depending on the value of the size of the window screen.
		 @param texture The Texture to be repositioned.
		 @param value The vertical offset from the middle, in pixels. */
		void RepositionTexture(const std::unique_ptr<Texture>& texture, float value);

		/* Allows the selection of the menu choices in the title screen. */
		void SelectMenuOption();

		/* Update the color of the menu choices when the mouse is hovering over the option. */
		void UpdateMenuOptionText();

	private:
		enum class TitleState { Splashscreen, MainMenu };

		enum class MenuOptions { Start, Exit };
		friend MenuOptions operator++(MenuOptions& menuOption);
		friend MenuOptions operator--(MenuOptions& menuOption);

	private:
		std::shared_ptr<Font> m_AbsEmpireFont;			// The title's font
		std::shared_ptr<Font> m_OpenSansFont;			// The menu's font

		std::unique_ptr<Texture> m_GroupNameTexture;	// The groups logo image
		std::unique_ptr<Texture> m_LogoTexture;			// Chesster's logo image

		std::unique_ptr<Texture> m_TitleText;
		std::unique_ptr<Texture> m_StartText;
		std::unique_ptr<Texture> m_ExitText;

		SDL_Point m_MousePos{ 0, 0 };
		Duration m_SplashDuration{};
		std::vector<const SDL_Rect*> m_MenuOptionsBounds;
		
		TitleState m_TitleState{ TitleState::Splashscreen };
		MenuOptions m_CurrentMenuOption{ MenuOptions::Start };
	};
}

#pragma once

#include "pch.h"

#include <SDL.h>
#include <SDL_image.h>

namespace Chesster
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Chesster!", unsigned int width = 720, unsigned int height = 720) :
			Title(title),
			Width(width),
			Height(height)
		{
		}
	};

	class Window
	{
	public:
		Window(const WindowProps& props);
		virtual ~Window();

		void OnUpdate();

		inline unsigned int GetWidth() const { return m_WinProps.Width; }
		inline unsigned int GetHeight() const { return m_WinProps.Height; }

		void LogSDLError(std::ostream& os, const std::string& msg);

		static Window* Create(const WindowProps& props = WindowProps());

	public:
		static SDL_Renderer* Renderer;

	private:
		bool Init(const WindowProps& props);
		void Close();

	private:
		WindowProps m_WinProps;

		SDL_Window* m_Window;
		SDL_Surface* m_ScreenSurface;
	};
}

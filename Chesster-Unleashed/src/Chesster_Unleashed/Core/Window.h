#pragma once

#include <string>

#include "Chesster_Unleashed/Renderer/Context.h"

struct SDL_Window;

namespace Chesster
{
	// Window Properties
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Gem Engine", uint32_t width = 1600, uint32_t height = 900) :
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

		const uint32_t& GetWidth() const { return m_WinProps.Width; }
		const uint32_t& GetHeight() const { return m_WinProps.Height; }

		static Window* Create(const WindowProps& props = WindowProps());

		virtual void* GetNativeWindow() const { return m_Window; }
		WindowProps* GetWinProps() { return &m_WinProps; }

	private:
		bool Init(const WindowProps& props);

	private:
		SDL_Window* m_Window;
		std::unique_ptr<Context> m_Context;
		WindowProps m_WinProps;
	};
}
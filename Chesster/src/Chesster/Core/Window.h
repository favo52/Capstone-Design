#pragma once

#include "Chesster/Renderer/Context.h"

namespace Chesster
{
	/// <summary>
	/// Window Properties. It stores the window's title, width and height.
	/// </summary>
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title, uint32_t width, uint32_t height) :
			Title{ title },
			Width{ width },
			Height{ height }
		{
		}
	};

	/// <summary>
	/// The Window class initialized the SDL library and its subsystems.
	/// It creates a window and a context for the application.
	/// </summary>
	class Window
	{
	public:
		/// <summary>
		/// It creates a window and a context for the application.
		/// </summary>
		/// <param name="props">The Window properties.</param>
		Window(const WindowProps& props);
		virtual ~Window();

		/// <summary>
		/// Updates the Window by swapping the buffers.
		/// </summary>
		void OnUpdate();

		/// <summary>
		/// Retrieves the Width from the WindowProps.
		/// </summary>
		/// <returns>The width of the Window.</returns>
		const uint32_t& GetWidth() const { return m_WinProps.Width; }

		/// <summary>
		/// Retrieves the Height from the WindowProps.
		/// </summary>
		/// <returns>The height of the Window.</returns>
		const uint32_t& GetHeight() const { return m_WinProps.Height; }

		/// <summary>
		/// Creates a new Window object with the specified properties.
		/// </summary>
		/// <param name="props">The WindowProps object containing the window properties.</param>
		/// <returns>A unique pointer of the created Window. It uses Window's constructor.</returns>
		static std::unique_ptr<Window> Create(const WindowProps& props);

		/// <summary>
		/// Used to retrieve the SDL_Window object.
		/// </summary>
		/// <returns>A pointer to the m_Window member variable.</returns>
		SDL_Window* GetSDLWindow() const { return m_Window; }

		/// <summary>
		/// Used to retrieve the WindowProps object.
		/// </summary>
		/// <returns>A pointer to the m_WinProps member variable.</returns>
		WindowProps* GetWinProps() { return &m_WinProps; }

	private:
		SDL_Window* m_Window;
		std::unique_ptr<Context> m_Context;
		WindowProps m_WinProps;
	};
}

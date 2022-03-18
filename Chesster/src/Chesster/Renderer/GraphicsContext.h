#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace Chesster
{
	/// <summary>
	/// Abstraction of the SDL Renderer.
	/// 
	/// </summary>
	class GraphicsContext
	{
	public:
		/// <summary>
		/// Creates a 2D renderering context for the given window.
		/// </summary>
		/// <param name="windowHandle">The window where rendering is displayed.</param>
		GraphicsContext(SDL_Window* windowHandle);

		/// <summary>
		/// Destroys the SDL renderer and SDL window handle.
		/// </summary>
		virtual ~GraphicsContext();

		/// <summary>
		/// Update the screen with any rendering performed since the previous call.
		/// </summary>
		void SwapBuffers();

	public:
		static SDL_Renderer* Renderer;

	private:
		SDL_Window* m_WindowHandle;
	};
}

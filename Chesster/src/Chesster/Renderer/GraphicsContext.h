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
		/// 
		/// </summary>
		/// <returns></returns>
		static SDL_Renderer* Renderer() { return s_Renderer; }

	private:
		static SDL_Renderer* s_Renderer;
	};
}

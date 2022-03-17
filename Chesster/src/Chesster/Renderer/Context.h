#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace Chesster
{
	/// <summary>
	/// Abstraction of the SDL Renderer.
	/// 
	/// </summary>
	class Context
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="windowHandle"></param>
		Context(SDL_Window* windowHandle);

		/// <summary>
		/// 
		/// </summary>
		virtual ~Context();

		/// <summary>
		/// 
		/// </summary>
		void SwapBuffers();

	public:
		static SDL_Renderer* Renderer;

	private:
		SDL_Window* m_WindowHandle;
	};
}

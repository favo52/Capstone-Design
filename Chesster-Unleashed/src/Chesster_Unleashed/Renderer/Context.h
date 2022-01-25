#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace Chesster
{
	class Context
	{
	public:
		Context(SDL_Window* windowHandle);
		virtual ~Context();

		void Init();
		void SwapBuffers();

	public:
		static SDL_Renderer* Renderer;

	private:
		SDL_Window* m_WindowHandle;
	};
}

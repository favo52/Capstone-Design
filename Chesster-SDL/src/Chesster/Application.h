#pragma once

#include "pch.h"

#include "Window.h"
#include "ResourceHolder.h"
#include "States/StateStack.h"

namespace Chesster
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		void Quit();

	private:
		void ProcessEvents();
		void Update();
		void Render();

		void HandleInput(const SDL_Event& e);

		void Cleanup();

		void RegisterStates();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_isRunning;

		Texture::Font m_Font;
		Texture m_FPSText;

		TextureHolder m_TextureHolder;
		FontHolder m_FontHolder;

		StateStack m_StateStack;

	};
}

#pragma once

#include "pch.h"

#include "Window.h"
#include "Time.h"
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
		void Update(const std::chrono::duration<double>& dt);
		void Render();

		void HandleInput(const SDL_Event& e);
		void CalculateFPS(const std::chrono::duration<double>& dt);

		void Cleanup();

		void RegisterStates();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_isRunning;

		// Frames per second
		std::chrono::duration<double> m_FPSUpdateTime;
		std::size_t m_FPSNumFrames;
		std::stringstream m_FPSNumberText;

		Texture::Font m_Font;
		Texture m_FPSText;

		TextureHolder m_TextureHolder;
		FontHolder m_FontHolder;

		StateStack m_StateStack;

	};
}

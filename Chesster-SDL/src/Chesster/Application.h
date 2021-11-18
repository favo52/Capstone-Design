#pragma once

#include "pch.h"

#include "Window.h"
#include "TextureHandler.h"

namespace Chesster
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		void ProcessEvents();
		void Update();
		void Render();

		void HandleInput(const SDL_Event& e);

		void Cleanup();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_isRunning;

		Texture* m_RSCLogo;

		TextureHandler<TexturesID> m_TextureHandler;
		TextureHandler<FontID> m_FontHandler;
	};
}

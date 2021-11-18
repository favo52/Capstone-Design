#include "pch.h"
#include "Application.h"

namespace Chesster
{
	Application::Application() :
		m_Window{ nullptr },
		m_isRunning{ true },
		m_RSCLogo{ nullptr },
		m_TextureHandler{}
	{
		m_Window = std::unique_ptr<Window>(Window::Create());

		m_TextureHandler.LoadFromFile(TexturesID::ReadySetCode, "resources/textures/ReadySetCode.jpeg");
		m_RSCLogo = &m_TextureHandler.Get(TexturesID::ReadySetCode);

		//m_FontHandler.LoadFromRenderedText();
	}

	Application::~Application()
	{
		Cleanup();
	}

	void Application::Run()
	{
		while (m_isRunning)
		{
			// Handle events on queue
			ProcessEvents();

			Update();

			Render();
		}
	}

	void Application::ProcessEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			// User requests quit
			if (e.type == SDL_QUIT)
				m_isRunning = false;

			// User presses a key
			else if (e.type == SDL_KEYDOWN)
				HandleInput(e);
		}
	}

	void Application::Update()
	{
		// Apply any Window updates
		m_Window->OnUpdate();

		// Update Game state

	}

	void Application::Render()
	{
		// Clear screen
		SDL_RenderClear(Window::Renderer);
		
		// Render texture to screen
		SDL_RenderCopy(Window::Renderer, m_RSCLogo->GetTexture(), NULL, NULL);

		// Update screen
		SDL_RenderPresent(Window::Renderer);
	}

	void Application::HandleInput(const SDL_Event& e)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_RETURN:
			break;
		case SDLK_ESCAPE:
			m_isRunning = false;
			break;
		case SDLK_SPACE:
			m_RSCLogo->SetColor(255u, 0u, 0u);
			break;
		case SDLK_BACKSPACE:
			m_RSCLogo->SetColor(255u, 255u, 255u);
			break;
		default:
			break;
		}
	}

	void Application::Cleanup()
	{
		// Free loaded image
		SDL_DestroyTexture(m_RSCLogo->GetTexture());
		m_RSCLogo = nullptr;
	}
}

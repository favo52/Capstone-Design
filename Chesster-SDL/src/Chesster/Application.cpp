#include "pch.h"
#include "Application.h"
#include "States/TitleState.h"
#include "States/MenuState.h"
#include "States/GameState.h"
#include "States/PauseState.h"

namespace Chesster
{
	Application::Application() :
		m_Window{ nullptr },
		m_isRunning{ true },
		m_Font{},
		m_FPSText{},
		m_TextureHolder{},
		m_FontHolder{},
		m_StateStack{ State::Context(m_Window, m_TextureHolder, m_FontHolder) }
	{
		m_Window = std::unique_ptr<Window>(Window::Create());

		// Prepare text
		m_FontHolder.Load(FontID::AbsEmpire_100, "resources/fonts/aAbsoluteEmpire.ttf", 100);
		m_FontHolder.Load(FontID::Minecraft, "resources/fonts/Minecraft.ttf");
		m_FontHolder.Load(FontID::Minecraft_100, "resources/fonts/Minecraft.ttf", 100);
		m_FontHolder.Load(FontID::Sansation, "resources/fonts/Sansation.ttf");
		m_FontHolder.Load(FontID::Sansation_10, "resources/fonts/Sansation.ttf", 10);
		m_FontHolder.Load(FontID::Sansation_100, "resources/fonts/Sansation.ttf", 100);
		
		// Prepare logos
		m_TextureHolder.Load(TextureID::ReadySetCode, "resources/textures/ReadySetCode.jpeg");
		m_TextureHolder.Load(TextureID::ChessterLogo, "resources/textures/ChessterLogo.jpeg");

		m_Font = m_FontHolder.Get(FontID::Sansation_10);
		m_FPSText.LoadFromRenderedText(m_Font, "FPS: ", { 0u, 0u, 0u });
		m_FPSText.SetPosition(5, 5);

		RegisterStates();
		m_StateStack.PushState(StateID::Title);
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

			// Check inside this loop, because stack might be empty before update() call
			if (m_StateStack.IsEmpty())
				Quit();

			Render();
		}
	}

	void Application::Quit()
	{
		m_isRunning = false;
	}

	void Application::ProcessEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			// User requests quit
			if (e.type == SDL_QUIT)
				Quit();
			/*else if (e.key.keysym.sym == SDLK_ESCAPE)
				Quit();*/

			m_StateStack.HandleEvent(e);
		}
	}

	void Application::Update()
	{
		// Apply any Window updates
		m_Window->OnUpdate();

		// Update states
		m_StateStack.Update();
	}

	void Application::Render()
	{
		// Clear screen
		SDL_RenderClear(Window::Renderer);

		m_StateStack.Draw();
		
		m_FPSText.Draw();

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
			//m_RSCLogo->SetColor(255u, 0u, 0u); // paint red
			break;
		case SDLK_BACKSPACE:
			//m_RSCLogo->SetColor(255u, 255u, 255u); // paint white
			break;
		default:
			break;
		}
	}

	void Application::Cleanup()
	{
		// Free loaded image
		m_FPSText.FreeTexture();

		// Free font
		TTF_CloseFont(m_Font.m_Font);
	}

	void Application::RegisterStates()
	{
		m_StateStack.RegisterState<TitleState>(StateID::Title);
		m_StateStack.RegisterState<MenuState>(StateID::Menu);
		//m_StateStack.RegisterState<SettingsState>(StateID::Settings);
		m_StateStack.RegisterState<GameState>(StateID::Gameplay);
		m_StateStack.RegisterState<PauseState>(StateID::Pause);
		//m_StateStack.RegisterState<GameOverState>(StateID::Gameover);
	}
}

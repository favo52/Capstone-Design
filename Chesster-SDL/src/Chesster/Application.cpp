#include "pch.h"
#include "Application.h"
#include "States/TitleState.h"
#include "States/MenuState.h"
#include "States/GameState.h"
#include "States/PauseState.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

namespace Chesster
{
	Application::Application() :
		m_Window{ nullptr },
		m_IsRunning{ true },
		m_FPSUpdateTime{},
		m_FPSNumFrames{ 0 },
		m_FPSNumberText{},
		m_Font{},
		m_FPSText{},
		m_TextureHolder{},
		m_FontHolder{},
		m_StateStack{ State::Context(m_Window, m_TextureHolder, m_FontHolder) }
	{
		m_Window = std::unique_ptr<Window>(Window::Create());

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForSDLRenderer(m_Window.get()->GetSDLWindow());
		ImGui_ImplSDLRenderer_Init(Window::Renderer);

		//io.Fonts->AddFontDefault();

		// Prepare fonts
		m_FontHolder.Load(FontID::AbsEmpire_100, "resources/fonts/aAbsoluteEmpire.ttf", 100);
		m_FontHolder.Load(FontID::Minecraft, "resources/fonts/Minecraft.ttf");
		m_FontHolder.Load(FontID::Minecraft_10, "resources/fonts/Minecraft.ttf", 12);
		m_FontHolder.Load(FontID::Minecraft_100, "resources/fonts/Minecraft.ttf", 100);
		m_FontHolder.Load(FontID::Sansation, "resources/fonts/Sansation.ttf");
		m_FontHolder.Load(FontID::Sansation_10, "resources/fonts/Sansation.ttf", 10);
		m_FontHolder.Load(FontID::Sansation_100, "resources/fonts/Sansation.ttf", 100);
		
		// Prepare logos
		m_TextureHolder.Load(TextureID::ReadySetCode, "resources/textures/ReadySetCode.jpeg");
		m_TextureHolder.Load(TextureID::ChessterLogo, "resources/textures/ChessterLogo.jpeg");

		// Set FPS text
		m_Font = m_FontHolder.Get(FontID::Minecraft_10);
		m_FPSText.LoadFromRenderedText(m_Font, "FPS  ", { 255u, 255u, 255u , 255u });
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
		// Fixed timestep
		using namespace std::literals;
		auto constexpr dt = 1.0s / 60.0;

		using duration = std::chrono::duration<double>;
		using time_point = std::chrono::time_point<Clock, duration>;

		time_point totalTime{};
		time_point currentTime = Clock::now();
		duration accumulator = 0s;

		while (m_IsRunning)
		{
			time_point newTime = Clock::now();
			auto frameTime = newTime - currentTime;
			if (frameTime > 0.25s)
				frameTime = 0.25s;
			currentTime = newTime;

			accumulator += frameTime;

			while (accumulator >= dt)
			{
				// Handle events on queue
				ProcessEvents();

				// Update game logic
				Update(dt);

				// Stack might be empty before update() call
				if (m_StateStack.IsEmpty())
					Quit();

				totalTime += dt;
				accumulator -= dt;
			}

			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame(m_Window.get()->GetSDLWindow());
			ImGui::NewFrame();

			// Calculate and correct fps
			//CalculateFPS(dt);

			// Draw everything
			Render();
		}
	}

	void Application::Quit()
	{
		m_IsRunning = false;
	}

	void Application::ProcessEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			// User requests quit
			if (e.type == SDL_QUIT)
				Quit();
			if (e.type == SDL_WINDOWEVENT &&
				e.window.event == SDL_WINDOWEVENT_CLOSE &&
				e.window.windowID == SDL_GetWindowID(m_Window.get()->GetSDLWindow()))
				Quit();

			m_StateStack.HandleEvent(e);
		}
	}

	void Application::Update(const std::chrono::duration<double>& dt)
	{
		// Update states
		m_StateStack.Update(dt);
	}

	void Application::Render()
	{
		// Clear screen
		SDL_RenderClear(Window::Renderer);

		m_StateStack.Draw();
		
		//m_FPSText.Draw();

		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

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
			m_IsRunning = false;
			break;
		case SDLK_SPACE:
			break;
		case SDLK_BACKSPACE:
			break;
		default:
			break;
		}
	}

	void Application::CalculateFPS(const std::chrono::duration<double>& dt)
	{
		using namespace std::chrono;
		static auto t = time_point_cast<seconds>(steady_clock::now());

		static int frame_count = 0;
		static int frame_rate = 0;
		auto pt = t;
		t = time_point_cast<seconds>(steady_clock::now());
		++frame_count;

		if (t != pt)
		{
			frame_rate = frame_count;
			frame_count = 0;
		}

		m_FPSNumberText.str("");
		m_FPSNumberText << "FPS  " << std::to_string(frame_rate);
		m_FPSText.LoadFromRenderedText(m_Font, m_FPSNumberText.str().c_str(), { 255u, 255u, 255u , 255u });
	}

	void Application::Cleanup()
	{
		// Free loaded image
		m_FPSText.FreeTexture();

		// Free font
		TTF_CloseFont(m_Font.m_Font);

		// Free ImGui
		ImGui_ImplSDLRenderer_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
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

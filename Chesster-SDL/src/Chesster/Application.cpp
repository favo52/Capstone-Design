#include "pch.h"
#include "Application.h"
#include "States/TitleState.h"
#include "States/MenuState.h"
#include "States/GameState.h"
#include "States/PauseState.h"
#include "States/GameOverState.h"

#include "SDL.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "Core/PlatformDetection.h"

namespace Chesster
{
	Application::Application() :
		m_Window{ nullptr },
		m_IsRunning{ true },
		m_TextureHolder{},
		m_FontHolder{},
		m_StateStack{ State::Context(m_Window, m_TextureHolder, m_FontHolder) }
	{
		#ifdef CHESSTER_PLATFORM_WINDOWS
			m_Window = std::unique_ptr<Window>(Window::Create());
		#elif CHESSTER_PLATFORM_ANDROID
			SDL_DisplayMode displayMode;
			if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
				m_Window = std::unique_ptr<Window>(Window::Create("CHESSTER", displayMode.w, displayMode.h));
		#endif

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Set ImGui font
		float fontSize = 18.0f;
		//io.Fonts->AddFontFromFileTTF("resources/fonts/OpenSans-Bold.ttf", fontSize);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForSDLRenderer(m_Window.get()->GetSDLWindow());
		ImGui_ImplSDLRenderer_Init(Window::Renderer);

		// Prepare fonts
		m_FontHolder.Load(FontID::AbsEmpire_100, "resources/fonts/aAbsoluteEmpire.ttf", 100);
		m_FontHolder.Load(FontID::OpenSans, "resources/fonts/OpenSans-Regular.ttf", 30);
		m_FontHolder.Load(FontID::OpenSans_100, "resources/fonts/OpenSans-Regular.ttf", 50);
		m_FontHolder.Load(FontID::Minecraft, "resources/fonts/Minecraft.ttf");
		m_FontHolder.Load(FontID::Minecraft_10, "resources/fonts/Minecraft.ttf", 12);
		m_FontHolder.Load(FontID::Minecraft_100, "resources/fonts/Minecraft.ttf", 100);
		m_FontHolder.Load(FontID::Sansation, "resources/fonts/Sansation.ttf");
		m_FontHolder.Load(FontID::Sansation_10, "resources/fonts/Sansation.ttf", 10);
		m_FontHolder.Load(FontID::Sansation_100, "resources/fonts/Sansation.ttf", 100);
		
		// Prepare logos
		m_TextureHolder.Load(TextureID::ReadySetCode, "resources/textures/ReadySetCode.jpeg");
		m_TextureHolder.Load(TextureID::ChessterLogo, "resources/textures/ChessterLogo.jpeg");

		RegisterStates();
		m_StateStack.PushState(StateID::Title);
	}

	Application::~Application()
	{
		Cleanup();
	}

	void Application::Run()
	{
		using namespace std::literals;
		using duration = std::chrono::duration<double>;
		using time_point = std::chrono::time_point<Clock, duration>;
		time_point currentTime = Clock::now();

		// Main App loop
		while (m_IsRunning)
		{
			// Handle events on queue
			ProcessEvents();

			// Variable delta time
			time_point newTime = Clock::now();
			auto frameTime = newTime - currentTime;
			currentTime = newTime;

			// Update game logic
			Update(frameTime);

			// Stack might be empty before update() call
			if (m_StateStack.IsEmpty())
				Quit();

			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame(m_Window.get()->GetSDLWindow());
			ImGui::NewFrame();

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

			// Handle events from all states
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
		
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		// Update screen
		SDL_RenderPresent(Window::Renderer);
	}

	void Application::Cleanup()
	{
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
		m_StateStack.RegisterState<GameOverState>(StateID::Gameover);
	}
}

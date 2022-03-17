#include "pch.h"
#include "Chesster/Core/Application.h"

#include "Chesster/Core/Clock.h"
#include "Chesster/Renderer/Renderer.h"

#include "Chesster/Layers/Gamelayer.h"

#include <backends/imgui_impl_sdl.h>

namespace Chesster
{
	Application* Application::s_Instance{ nullptr };

	Application::Application(const std::string& name) :
		m_Running{ true },
		m_Minimized{ false },
		m_Fullscreen{ false }
	{
		CHESSTER_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Initialize the logger
		Chesster::Logger::Init();
		LOG_INFO("Welcome to the Chesster Universal Chess Interface!");

		// Create the window
		m_Window = Window::Create(WindowProps(name, 1600, 900));

		// Load the resources
		m_TextureHolder.Load(TextureID::GroupLogo, "assets/textures/ReadySetCode.jpeg");
		m_TextureHolder.Load(TextureID::ChessterLogo, "assets/textures/ChessterLogo.png");
		m_TextureHolder.Load(TextureID::Pieces, "assets/textures/ChessPieces.png");

		m_FontHolder.Load(FontID::OpenSans, "assets/fonts/opensans/OpenSans-Regular.ttf");
		m_FontHolder.Load(FontID::OpenSans_Bold, "assets/fonts/opensans/OpenSans-Bold.ttf");
		m_FontHolder.Load(FontID::AbsEmpire, "assets/fonts/aAbsoluteEmpire.ttf", 100);

		// Create the layers
		m_TitleLayer = new TitleLayer();
		PushLayer(m_TitleLayer);
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::Run()
	{
		using namespace std::literals;
		using Duration = std::chrono::duration<double>;
		using TimePoint = std::chrono::time_point<Clock, Duration>;
		TimePoint currentTime = Clock::Now();

		// Main App loop
		while (m_Running)
		{
			// Handle events on queue
			SDL_Event e;
			while (SDL_PollEvent(&e))
				OnEvent(e);

			// Hold loop if minimized
			if (m_Minimized) SDL_WaitEvent(&e);

			// Variable delta time
			TimePoint newTime = Clock::Now();
			auto frameTime = newTime - currentTime;
			currentTime = newTime;

			// Update game logic
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(frameTime);

			// Draw everything
			for (Layer* layer : m_LayerStack)
				layer->OnRender();

			// Render ImGui on top
			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			// Update screen
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(SDL_Event& sdlEvent)
	{
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		// User presses window's X
		if (sdlEvent.type == SDL_QUIT)
			Quit();

		// Fullscreen
		if (sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_F5 && sdlEvent.key.repeat == 0)
			ToggleFullscreen();

		// Window events
		if (sdlEvent.type == SDL_WINDOWEVENT)
			OnWindowEvent(sdlEvent);

		// Handle layer pop/push
		if (TitleLayer::IsStart)
		{
			PopLayer(m_TitleLayer);
			PushLayer(new GameLayer);
			TitleLayer::IsStart = false;
			delete m_TitleLayer;
		}

		// Handle layer events
		for (auto itr = m_LayerStack.rbegin(); itr != m_LayerStack.rend(); ++itr)
			(*itr)->OnEvent(sdlEvent);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::PopOverlay(Layer* overlay)
	{
		m_LayerStack.PopOverlay(overlay);
		overlay->OnDetach();
	}

	void Application::ToggleFullscreen()
	{
		SDL_Window* window = m_Window->GetSDLWindow();

		if (m_Fullscreen)
		{
			SDL_SetWindowFullscreen(window, 0);

			int width{ 0 }, height{ 0 };
			SDL_GetWindowSize(window, &width, &height);
			Renderer::OnWindowResize(width, height);

			m_Fullscreen = false;
		}
		else
		{
			int displayIndex = SDL_GetWindowDisplayIndex(window);
			SDL_DisplayMode displayMode = { (uint32_t)0, (int)m_Window->GetWidth(), (int)m_Window->GetHeight(), 0, nullptr };
			if (SDL_GetDesktopDisplayMode(displayIndex, &displayMode) < 0)
				LOG_WARN("SDL_GetDesktopDisplayMode failed with error: {0}", SDL_GetError());

			SDL_SetWindowDisplayMode(window, &displayMode);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			m_Fullscreen = true;
			m_Minimized = false;
		}
	}

	void Application::OnWindowEvent(SDL_Event& sdlEvent)
	{
		// Get the window ID, display error if failed
		uint32_t windowID = SDL_GetWindowID(m_Window->GetSDLWindow());
		if (windowID == 0) LOG_ERROR("Application::OnWindowEvent(SDL_Event&) - SDL_GetWindowID failed with error: {0}", SDL_GetError());

		if (sdlEvent.window.windowID == windowID)
		{
			// Handle the window events
			switch (sdlEvent.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					Quit();
					break;

				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					m_Window->GetWinProps()->Width = sdlEvent.window.data1;
					m_Window->GetWinProps()->Height = sdlEvent.window.data2;
					break;
				}

				case SDL_WINDOWEVENT_RESIZED:
					Renderer::OnWindowResize(sdlEvent.window.data1, sdlEvent.window.data2);
					break;

				case SDL_WINDOWEVENT_EXPOSED:
					m_Window->OnUpdate();
					break;

				case SDL_WINDOWEVENT_MINIMIZED:
					m_Minimized = true;
					break;

				case SDL_WINDOWEVENT_MAXIMIZED:
					m_Minimized = false;
					break;

				case SDL_WINDOWEVENT_RESTORED:
					m_Minimized = false;
					break;

				default:
					break;
			}
		}
	}
}

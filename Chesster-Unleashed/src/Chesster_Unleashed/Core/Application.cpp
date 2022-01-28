#include "pch.h"
#include "Chesster_Unleashed/Core/Application.h"

#include "Chesster_Unleashed/Core/Clock.h"
#include "Chesster_Unleashed/Renderer/Renderer.h"
#include "Chesster_Unleashed/Layers/TitleLayer.h"
#include "Chesster_Unleashed/Layers/GameLayer.h"

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

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));

		m_TextureHolder.Load(TextureID::GroupLogo, "assets/textures/ReadySetCode.jpeg");
		m_TextureHolder.Load(TextureID::ChessterLogo, "assets/textures/ChessterLogo.png");
		m_TextureHolder.Load(TextureID::Pieces, "assets/textures/ChessPieces.png");

		//PushLayer(new TitleLayer);
		PushLayer(new GameLayer);

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

			if (!m_Minimized)
			{
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
			}

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
		if (sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_F5)
			ToggleFullscreen();

		// Window events
		if (sdlEvent.type == SDL_WINDOWEVENT)
			OnWindowEvent(sdlEvent);

		// Handle layer events
		for (auto itr = m_LayerStack.rbegin(); itr != m_LayerStack.rend(); ++itr)
			(*itr)->OnEvent(sdlEvent);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::ToggleFullscreen()
	{
		SDL_Window* window = (SDL_Window*)m_Window->GetNativeWindow();

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
				CHESSTER_WARN("SDL_GetDesktopDisplayMode failed with error: {0}", SDL_GetError());

			SDL_SetWindowDisplayMode(window, &displayMode);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			m_Fullscreen = true;
			m_Minimized = false;
		}
	}

	void Application::OnWindowEvent(SDL_Event& sdlEvent)
	{
		uint32_t windowID = SDL_GetWindowID(static_cast<SDL_Window*>(m_Window->GetNativeWindow()));
		if (windowID == 0) CHESSTER_ERROR("Application::OnWindowEvent(SDL_Event&) - SDL_GetWindowID failed with error: {0}", SDL_GetError());

		switch (sdlEvent.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE:
				if (sdlEvent.window.windowID == windowID)
					Quit();
				break;

			case SDL_WINDOWEVENT_SIZE_CHANGED:
				if (sdlEvent.window.windowID == windowID)
				{
					m_Window->GetWinProps()->Width = sdlEvent.window.data1;
					m_Window->GetWinProps()->Height = sdlEvent.window.data2;
					m_Window->OnUpdate();
				}
				break;

			case SDL_WINDOWEVENT_RESIZED:
				if (sdlEvent.window.windowID == windowID)
					Renderer::OnWindowResize(sdlEvent.window.data1, sdlEvent.window.data2);
				break;

			case SDL_WINDOWEVENT_EXPOSED:
				if (sdlEvent.window.windowID == windowID)
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

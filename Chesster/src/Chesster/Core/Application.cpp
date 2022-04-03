#include "pch.h"
#include "Chesster/Core/Application.h"

#include "Chesster/Core/Clock.h"

#include "Chesster/Layers/TitleLayer.h"
#include "Chesster/Layers/Gamelayer.h"
#include "Chesster/ImGui/ImGuiLayer.h"

#include <backends/imgui_impl_sdl.h>

namespace Chesster
{
	Application* Application::s_Instance{ nullptr };

	Application::Application(const std::string& name) :
		m_Running{ true }
	{
		s_Instance = this;

		// Initialize the logger
		Logger::Init();
		LOG_INFO("Welcome to the Chesster Universal Chess Interface!");

		// Create the window
		m_Window = std::make_unique<Window>(WindowProps(name, 1600, 900));

		// Create the layers
		m_TitleLayer = new TitleLayer();
		PushLayer(m_TitleLayer);
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::Run()
	{
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

			// Handle layer push/pop
			OnLayerEvent();

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
			m_Window->SwapBuffers();
		}
	}

	void Application::OnEvent(SDL_Event& sdlEvent)
	{
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		// User presses window's X
		if (sdlEvent.type == SDL_QUIT)
			Quit();

		// Window events
		if (sdlEvent.type == SDL_WINDOWEVENT)
			m_Window->OnWindowEvent(sdlEvent);		

		// Handle layer events
		for (auto itr = m_LayerStack.rbegin(); itr != m_LayerStack.rend(); ++itr)
			(*itr)->OnEvent(sdlEvent);
	}

	void Application::OnLayerEvent()
	{		
		if (TitleLayer::IsStart)
		{
			TitleLayer::IsStart = false;
			m_LayerStack.PopLayer(m_TitleLayer);
			delete m_TitleLayer;
			m_TitleLayer = nullptr;

			PushLayer(new GameLayer);
		}
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
}

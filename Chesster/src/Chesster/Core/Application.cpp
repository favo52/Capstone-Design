#include "pch.h"
#include "Chesster/Core/Application.h"

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
		m_TitleLayer = std::make_shared<TitleLayer>();
		PushLayer(m_TitleLayer);
		
		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		PushLayer(m_ImGuiLayer);
	}

	Application::~Application()
	{
		for (auto& layer : m_LayerStack)
			layer->OnDetach();

		m_LayerStack.clear();
	}

	void Application::Run()
	{
		using Clock = std::chrono::steady_clock;
		using Duration = std::chrono::duration<double>;
		using TimePoint = std::chrono::time_point<Clock, Duration>;

		TimePoint currentTime = Clock::now();

		// Main App loop
		while (m_Running)
		{
			// Handle events on queue
			SDL_Event e;
			while (SDL_PollEvent(&e))
				OnEvent(e);

			// Variable delta time
			TimePoint newTime = Clock::now();
			auto frameTime = newTime - currentTime;
			currentTime = newTime;

			// Update game logic
			for (auto& layer : m_LayerStack)
				layer->OnUpdate(frameTime);

			// Draw everything
			for (auto& layer : m_LayerStack)
				layer->OnRender();

			// Render ImGui on top
			m_ImGuiLayer->Begin();
			{
				for (auto& layer : m_LayerStack)
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
		for (auto& layer : m_LayerStack)
			layer->OnEvent(sdlEvent);

		// Handle layer push/pop
		if (TitleLayer::s_IsStart)
		{
			TitleLayer::s_IsStart = false;
			PopLayer(m_TitleLayer);
			m_TitleLayer.reset();

			m_GameLayer = std::make_shared<GameLayer>();
			PushLayer(m_GameLayer);
		}
	}

	void Application::PushLayer(const std::shared_ptr<Layer>& layer)
	{
		m_LayerStack.emplace_back(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(const std::shared_ptr<Layer>& layer)
	{
		auto itr = std::find(m_LayerStack.begin(), m_LayerStack.end(), layer);
		if (itr != m_LayerStack.end())
		{
			layer->OnDetach();
			m_LayerStack.erase(itr);
		}
	}
}

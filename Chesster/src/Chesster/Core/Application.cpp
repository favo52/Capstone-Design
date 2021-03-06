/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "pch.h"
#include "Chesster/Core/Application.h"

#include "Chesster/Layers/TitleLayer.h"
#include "Chesster/Layers/ImGuiLayer.h"

#include <backends/imgui_impl_sdl.h>

namespace Chesster
{
	Application* Application::s_Instance{ nullptr };

	Application::Application(const std::string& name) :
		m_IsRunning{ true }
	{
		assert(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Initialize the logger
		Logger::Init();
		LOG_INFO("Welcome to the Chesster Universal Chess Interface!");
		
		// Create the window
		m_Window = std::make_unique<Window>(WindowProps(name, 1280, 720));

		// Create the starting layers
		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		PushLayer(m_ImGuiLayer);

		PushLayer(std::make_shared<TitleLayer>());
	}

	Application::~Application()
	{
		for (auto& layer : m_LayerStack)
			layer->OnDetach();

		m_LayerStack.clear();
	}

	void Application::RequestLayerAction(Layer::Action layerAction, const std::shared_ptr<Layer>& layer)
	{
		m_PendingChanges.emplace_back(PendingChange{ layerAction, layer });
	}

	void Application::Run()
	{
		using Clock = std::chrono::steady_clock;
		using TimePoint = std::chrono::time_point<Clock, Duration>;

		TimePoint currentTime = Clock::now();

		// Main program loop
		while (m_IsRunning)
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

		// User presses the window's X (Close)
		if (sdlEvent.type == SDL_QUIT)
			Quit();

		// Window events
		if (sdlEvent.type == SDL_WINDOWEVENT)
			m_Window->OnWindowEvent(sdlEvent);

		// Handle layer events
		for (auto& layer : m_LayerStack)
			layer->OnEvent(sdlEvent);

		// Apply pending layer changes (pushes and pops)
		for (auto& pendingChange : m_PendingChanges)
		{
			switch (pendingChange.Action)
			{
				case Layer::Action::PushLayer:
					PushLayer(pendingChange.Layer);
					break;

				case Layer::Action::PopLayer:
					PopLayer(pendingChange.Layer);
					break;
			}
		}
		m_PendingChanges.clear();
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

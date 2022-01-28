#pragma once

#include "Chesster_Unleashed/Core/Core.h"
#include "Chesster_Unleashed/Core/Window.h"
#include "Chesster_Unleashed/Core/LayerStack.h"
#include "Chesster_Unleashed/Core/ResourceHolder.h"

#include "Chesster_Unleashed/ImGui/ImGuiLayer.h"

namespace Chesster
{
	class Application
	{
	public:
		Application(const std::string& name = "CHESSTER UNLEASHED");
		virtual ~Application() = default;

		void Run();
		void Quit() { m_Running = false; }

		void OnEvent(SDL_Event& sdlEvent);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_Window; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }

	public:
		TextureHolder m_TextureHolder;

	private:
		void ToggleFullscreen();
		void OnWindowEvent(SDL_Event& sdlEvent);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
		bool m_Minimized;
		bool m_Fullscreen;

		LayerStack m_LayerStack;

		ImGuiLayer* m_ImGuiLayer;

	private:
		static Application* s_Instance;
	};
}

#pragma once

#include "pch.h"

#include "Core/Window.h"
#include "Core/Time.h"
#include "Core/ResourceHolder.h"
#include "States/StateStack.h"

namespace Chesster
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		void Quit();

	private:
		void ProcessEvents();
		void Update(const std::chrono::duration<double>& dt);
		void Render();

		void Cleanup();
		void RegisterStates();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_IsRunning;
		bool m_IsFullScreen;
		bool m_IsMinimized;

		// Resource holders
		TextureHolder m_TextureHolder;
		FontHolder m_FontHolder;

		StateStack m_StateStack;
	};
}

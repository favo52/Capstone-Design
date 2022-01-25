#include "pch.h"
#include "Chesster_Unleashed/Core/Window.h"

#include "Chesster_Unleashed/Renderer/Context.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace Chesster
{
	Window::Window(const WindowProps& props) :
		m_Window{ nullptr },
		m_Context{ nullptr },
		m_WinProps{ props }
	{
		if (!Init(props)) throw std::runtime_error("Failed to initialize the Window!\n");
	}

	Window::~Window()
	{
		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;

		// Quit SDL subsystems
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();

		CHESSTER_INFO("SDL closed successfully.")
	}

	void Window::OnUpdate()
	{
		m_Context->SwapBuffers();
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new Window(props);
	}

	bool Window::Init(const WindowProps& props)
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
		{
			CHESSTER_ERROR("SDL_Init failed with error: {0}", SDL_GetError());
			return false;
		}

		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			CHESSTER_WARN("Warning: Linear texture filtering not enabled!");

		SDL_WindowFlags WindowFlags = (SDL_WindowFlags)
			(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		// Create window with OpenGL graphics context
		CHESSTER_INFO("Creating SDL Window: {0} ({1}, {2})", props.Title, props.Width, props.Height);
		m_Window = SDL_CreateWindow(props.Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, props.Width, props.Height, WindowFlags);
		if (m_Window == nullptr)
		{
			CHESSTER_ERROR("SDL_CreateWindow failed with error: {0}", SDL_GetError());
			return false;
		}

		// Initialize context
		m_Context = std::make_unique<Context>(static_cast<SDL_Window*>(m_Window));
		m_Context->Init();

		// Initialize PNG and JPG loading
		int imgFlagsPNG = IMG_INIT_PNG;
		int imgFlagsJPG = IMG_INIT_JPG;
		if (!(IMG_Init(imgFlagsPNG) & imgFlagsPNG) || !(IMG_Init(imgFlagsJPG) & imgFlagsJPG))
		{
			CHESSTER_ERROR("SDL_image could not initialize! SDL_image error: {0}", IMG_GetError());
			return false;
		}

		// Initialize SDL_ttf
		if (TTF_Init() < 0)
		{
			CHESSTER_ERROR("SDL_ttf could not initialize! SDL_ttf error: {0}", TTF_GetError());
			return false;
		}

		return true;
	}
}

#include "pch.h"
#include "Window.h"

namespace Chesster
{
	SDL_Renderer* Window::Renderer = nullptr;

	Window::Window(const WindowProps& props) :
		m_Window{ nullptr },
		m_ScreenSurface{ nullptr },
		m_WinProps{ props }
	{
		if (!Init(props))
			throw std::runtime_error("Failed to initialize Window!\n");
	}

	Window::~Window()
	{
		Close();
	}

	void Window::OnUpdate()
	{
		// Fill the surface white
		//SDL_FillRect(m_ScreenSurface, NULL, SDL_MapRGB(m_ScreenSurface->format, 0xFF, 0xFF, 0xFF));

		// Update the surface
		//SDL_UpdateWindowSurface(m_Window);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new Window(props);
	}

	bool Window::Init(const WindowProps& props)
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			LogSDLError(std::cout, "Init");
			return false;
		}

		// Create window
		m_Window = SDL_CreateWindow(props.Title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, props.Width, props.Height, SDL_WINDOW_SHOWN);
		if (m_Window == nullptr)
		{
			LogSDLError(std::cout, "Window");
			return false;
		}

		// Setup renderer
		Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (Renderer == nullptr)
		{
			LogSDLError(std::cout, "Renderer");
			return false;
		}

		// Initialize renderer color
		SDL_SetRenderDrawColor(Renderer, 255u, 255u, 255u, 255u); // White

		// Initialize PNG and JPG loading
		int imgFlagsPNG = IMG_INIT_PNG;
		int imgFlagsJPG = IMG_INIT_JPG;
		if (!(IMG_Init(imgFlagsPNG) & imgFlagsPNG) || !(IMG_Init(imgFlagsJPG) & imgFlagsJPG))
		{
			std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError << '\n';
			return false;
		}

		// Get window surface
		m_ScreenSurface = SDL_GetWindowSurface(m_Window);

		return true;
	}

	void Window::Close()
	{
		// Destroy renderer and window
		SDL_DestroyRenderer(Renderer);
		Renderer = nullptr;

		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;

		// Quit SDL subsystems
		IMG_Quit();
		SDL_Quit();
	}

	void Window::LogSDLError(std::ostream& os, const std::string& msg)
	{
		os << msg << " SDL Error: " << SDL_GetError() << std::endl;
	}
}

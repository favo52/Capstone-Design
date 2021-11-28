#include "pch.h"
#include "Window.h"

namespace Chesster
{
	SDL_Renderer* Window::Renderer = nullptr;

	Window::Window(const WindowProps& props) :
		m_Window{ nullptr },
		m_WinProps{ props }
	{
		if (!Init(props))
			throw std::runtime_error("Failed to initialize Window!\n");
	}
	
	Window::~Window()
	{
		Close();
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new Window(props);
	}

	bool Window::Init(const WindowProps& props)
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		{
			CHESSTER_ERROR("SDL_Init failed with error: {0}", SDL_GetError())
			return false;
		}

		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			CHESSTER_WARN("Warning: Linear texture filtering not enabled!");

		// Create window
		SDL_WindowFlags WindowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
		m_Window = SDL_CreateWindow
		(
			props.Title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			props.Width, props.Height,
			WindowFlags
		);
		if (m_Window == nullptr)
		{
			CHESSTER_ERROR("SDL_CreateWindow failed with error: {0}", SDL_GetError());
			return false;
		}

		// Create vsynced renderer for window
		Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (Renderer == nullptr)
		{
			CHESSTER_ERROR("SDL_CreateRenderer failed with error: {0}", SDL_GetError());
			return false;
		}
		SDL_RendererInfo info;
		SDL_GetRendererInfo(Renderer, &info);
		CHESSTER_INFO("Current SDL_Renderer: {0}", info.name);

		// Initialize renderer color
		SDL_SetRenderDrawColor(Renderer, 255u, 255u, 255u, 255u); // White

		// Initialize PNG and JPG loading
		int imgFlagsPNG = IMG_INIT_PNG;
		int imgFlagsJPG = IMG_INIT_JPG;
		if (!(IMG_Init(imgFlagsPNG) & imgFlagsPNG) || !(IMG_Init(imgFlagsJPG) & imgFlagsJPG))
		{
			CHESSTER_ERROR("IMG_Init failed with error: {0}", IMG_GetError());
			return false;
		}

		// Initialize SDL_ttf
		if (TTF_Init() < 0)
		{
			CHESSTER_ERROR("TTF_Init failed with error: {0}", TTF_GetError());
			return false;
		}

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
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}
}

#include "pch.h"
#include "Chesster/Core/Window.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

namespace Chesster
{
	// Disguises std::runtime_error()
	inline void RuntimeError(const std::string& s) { throw std::runtime_error(s); }

	Window::Window(const WindowProps& props) :
		m_Window{ nullptr },
		m_WinProps{ props }
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
		{
			LOG_ERROR("SDL_Init failed with error: {0}", SDL_GetError());
			RuntimeError("SDL failed to initialize. Verify Chesster.log for more info.");
		}

		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			LOG_WARN("Warning: Linear texture filtering not enabled!");

		// Set window creation flags
		SDL_WindowFlags windowFlags = (SDL_WindowFlags)
			(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
				| SDL_WINDOW_MAXIMIZED | SDL_WINDOW_ALLOW_HIGHDPI);

		// Create window with OpenGL graphics context
		m_Window = SDL_CreateWindow(
			m_WinProps.Title.c_str(),	// Window title
			SDL_WINDOWPOS_CENTERED,		// Initial top left x position
			SDL_WINDOWPOS_CENTERED,		// Initial top left y position
			m_WinProps.Width,			// Width, in pixels
			m_WinProps.Height,			// Height, in pixels
			windowFlags					// Flags
		);
		if (m_Window == nullptr) // Error handling
		{
			LOG_ERROR("SDL_CreateWindow failed with error: {0}", SDL_GetError());
			RuntimeError("Failed to create window. Verify Chesster.log for more info.");
		}
		LOG_INFO("Created SDL Window: {0} ({1}, {2})", m_WinProps.Title, m_WinProps.Width, m_WinProps.Height);

		// Initialize graphics context (SDL_Renderer)
		m_GraphicsContext = std::make_unique<Renderer>(m_Window);

		// Initialize SDL_image with PNG and JPG loading
		int imgFlagsPNG = IMG_INIT_PNG;
		int imgFlagsJPG = IMG_INIT_JPG;
		if (!(IMG_Init(imgFlagsPNG) & imgFlagsPNG) || !(IMG_Init(imgFlagsJPG) & imgFlagsJPG))
		{
			LOG_ERROR("SDL_image could not initialize! SDL_image error: {0}", IMG_GetError());
			RuntimeError("Failed to initialize SDL_image. Verify Chesster.log for more info.");
		}

		// Initialize SDL_ttf
		if (TTF_Init() < 0)
		{
			LOG_ERROR("SDL_ttf could not initialize! SDL_ttf error: {0}", TTF_GetError());
			RuntimeError("Failed to initialize SDL_ttf. Verify Chesster.log for more info.");
		}
	}

	Window::~Window()
	{
		// Destroy the SDL_Renderer first
		m_GraphicsContext.reset();

		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;

		// Quit SDL subsystems
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();

		LOG_INFO("SDL closed successfully.");
	}

	void Window::SwapBuffers()
	{
		SDL_RenderPresent(m_GraphicsContext->Get());
	}

	void Window::OnWindowEvent(SDL_Event& sdlEvent)
	{
		// Handle the window events
		switch (sdlEvent.window.event)
		{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			{
				m_WinProps.Width = sdlEvent.window.data1;
				m_WinProps.Height = sdlEvent.window.data2;
				break;
			}

			case SDL_WINDOWEVENT_RESIZED:
				m_GraphicsContext->SetViewport(0, 0, sdlEvent.window.data1, sdlEvent.window.data2);
				break;

			case SDL_WINDOWEVENT_EXPOSED:
				SwapBuffers();
				break;
		}
	}
}

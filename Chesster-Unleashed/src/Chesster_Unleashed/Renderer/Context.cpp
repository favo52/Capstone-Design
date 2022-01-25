#include "pch.h"
#include "Context.h"

#include <SDL_render.h>
#include <SDL_hints.h>

namespace Chesster
{
	SDL_Renderer* Context::Renderer{ nullptr };

	Context::Context(SDL_Window* windowHandle) :
		m_WindowHandle{ windowHandle }
	{
	}

	Context::~Context()
	{
		// Destroy renderer and window
		SDL_DestroyRenderer(Renderer);
		Renderer = nullptr;

		SDL_DestroyWindow(m_WindowHandle);
		m_WindowHandle = nullptr;
	}

	void Context::Init()
	{
		//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

		// Create vsynced renderer for window
		Renderer = SDL_CreateRenderer(m_WindowHandle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (Renderer == nullptr)
		{
			CHESSTER_ERROR("SDL_CreateRenderer failed with error: {0}", SDL_GetError());
			return;
		}
		SDL_RendererInfo info;
		SDL_GetRendererInfo(Renderer, &info);
		CHESSTER_INFO("Current SDL_Renderer: {0} | Texture formats: {1}", info.name, info.num_texture_formats);
		
		// Initialize renderer color
		SDL_SetRenderDrawColor(Renderer, 25u, 25u, 25u, 255u);
	}

	void Context::SwapBuffers()
	{
		SDL_RenderPresent(Renderer);
	}
}

#include "pch.h"
#include "GraphicsContext.h"

#include <SDL_render.h>
#include <SDL_hints.h>

namespace Chesster
{
	SDL_Renderer* GraphicsContext::Renderer{ nullptr };

	GraphicsContext::GraphicsContext(SDL_Window* windowHandle) :
		m_WindowHandle{ windowHandle }
	{
		// Create vsynced renderer for window
		Renderer = SDL_CreateRenderer(m_WindowHandle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (Renderer == nullptr)
		{
			LOG_ERROR("SDL_CreateRenderer failed with error: {0}", SDL_GetError());
			throw std::runtime_error("SDL_CreateRenderer failed with error: " + std::string(SDL_GetError()));
		}
		SDL_RendererInfo info{};
		SDL_GetRendererInfo(Renderer, &info);
		LOG_INFO("Current SDL_Renderer: {0} | Texture formats: {1}", info.name, info.num_texture_formats);

		// Set renderer settings
		SDL_SetRenderDrawColor(Renderer, 255u, 255u, 255u, 255u);
		SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	}

	GraphicsContext::~GraphicsContext()
	{
		// Destroy renderer and window
		SDL_DestroyRenderer(Renderer);
		Renderer = nullptr;

		SDL_DestroyWindow(m_WindowHandle);
		m_WindowHandle = nullptr;
	}

	void GraphicsContext::SwapBuffers()
	{
		SDL_RenderPresent(Renderer);
	}
}

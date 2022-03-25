#include "pch.h"
#include "Chesster/Renderer/GraphicsContext.h"

#include <SDL_render.h>

namespace Chesster
{
	SDL_Renderer* GraphicsContext::s_Renderer{ nullptr };

	GraphicsContext::GraphicsContext(SDL_Window* windowHandle)
	{
		// Create vsynced renderer for window
		s_Renderer = SDL_CreateRenderer(windowHandle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (s_Renderer == nullptr)
		{
			LOG_ERROR("SDL_CreateRenderer failed with error: {0}", SDL_GetError());
			throw std::runtime_error("SDL_CreateRenderer failed. Verify Chesster.log for more info.");
		}
		SDL_RendererInfo info{};
		SDL_GetRendererInfo(s_Renderer, &info);
		LOG_INFO("Current SDL_Renderer: {0} | Texture formats: {1}", info.name, info.num_texture_formats);

		// Set renderer settings
		SDL_SetRenderDrawColor(s_Renderer, 255u, 255u, 255u, 255u);
		SDL_SetRenderDrawBlendMode(s_Renderer, SDL_BLENDMODE_BLEND);
	}

	GraphicsContext::~GraphicsContext()
	{
		// Destroy renderer
		SDL_DestroyRenderer(s_Renderer);
		s_Renderer = nullptr;
	}
}

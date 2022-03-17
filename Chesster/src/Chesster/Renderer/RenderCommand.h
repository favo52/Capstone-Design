#pragma once

#include "Chesster/Renderer/Context.h"

#include <SDL_render.h>

#include <glm/glm.hpp>

namespace Chesster
{
	class RenderCommand
	{
	public:
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			const SDL_Rect viewport = { (int)x, (int)y, (int)width, (int)height };
			SDL_RenderSetViewport(Context::Renderer, &viewport);
		}

		static void SetClearColor(const glm::vec4& color = { 25, 25, 25, 255 })
		{
			SDL_SetRenderDrawColor(Context::Renderer, (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
		}

		static void Clear()
		{
			SDL_RenderClear(Context::Renderer);
		}
	};
}

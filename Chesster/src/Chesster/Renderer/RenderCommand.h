#pragma once

#include "Chesster/Renderer/GraphicsContext.h"

#include <SDL_render.h>

#include <glm/glm.hpp>

namespace Chesster
{
	/// <summary>
	/// Wrapper class of some SDL functions like setting viewport,
	/// setting the clear color and clearing the screen.
	/// </summary>
	class RenderCommand
	{
	public:
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			const SDL_Rect viewport = { (int)x, (int)y, (int)width, (int)height };
			SDL_RenderSetViewport(GraphicsContext::Renderer, &viewport);
		}

		static void SetClearColor(const glm::vec4& color = { 25, 25, 25, 255 })
		{
			SDL_SetRenderDrawColor(GraphicsContext::Renderer, (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
		}

		static void Clear()
		{
			SDL_RenderClear(GraphicsContext::Renderer);
		}
	};
}

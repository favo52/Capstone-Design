/*
* Copyright 2022-present, Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#include "pch.h"
#include "Chesster/Renderer/Renderer.h"

#include <SDL_render.h>

namespace Chesster
{
	SDL_Renderer* Renderer::s_Renderer{ nullptr };

	Renderer::Renderer(SDL_Window* windowHandle)
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
		SDL_SetRenderDrawColor(s_Renderer, 255u, 255u, 255u, SDL_ALPHA_OPAQUE);
		SDL_SetRenderDrawBlendMode(s_Renderer, SDL_BLENDMODE_BLEND);
	}

	Renderer::~Renderer()
	{
		SDL_DestroyRenderer(s_Renderer);
		s_Renderer = nullptr;

		LOG_INFO("Renderer destroyed successfully.");
	}

	void Renderer::DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color)
	{
		SetClearColor(color);
		SDL_RenderFillRect(s_Renderer, &rect);
	}

	void Renderer::DrawTexture(const std::unique_ptr<Texture>& texture)
	{
		SDL_RenderCopy(s_Renderer, texture->GetSDLTexture(), texture->GetRenderClip(), &texture->GetBounds());
	}
	
	void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		const SDL_Rect viewport = { (int)x, (int)y, (int)width, (int)height };
		SDL_RenderSetViewport(s_Renderer, &viewport);
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		SDL_SetRenderDrawColor(s_Renderer, (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
	}

	void Renderer::Clear()
	{
		SDL_RenderClear(s_Renderer);
	}

	RectBounds RectBounds::operator+(const float& value) const
	{
		return { left + value, right + value, bottom + value, top + value };
	}
}

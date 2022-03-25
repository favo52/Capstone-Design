#include "pch.h"
#include "Chesster/Renderer/Renderer.h"

#include "Chesster/Renderer/GraphicsContext.h"

namespace Chesster
{
	void Renderer::DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color)
	{
		SetClearColor(color);
		SDL_RenderFillRect(GraphicsContext::Renderer(), &rect);
	}

	void Renderer::DrawTexture(const Texture* texture)
	{
		SDL_RenderCopy(GraphicsContext::Renderer(), texture->GetSDLTexture(), texture->GetRenderClip(), &texture->GetBounds());
	}

	void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		const SDL_Rect viewport = { (int)x, (int)y, (int)width, (int)height };
		SDL_RenderSetViewport(GraphicsContext::Renderer(), &viewport);
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		SDL_SetRenderDrawColor(GraphicsContext::Renderer(), (Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a);
	}

	void Renderer::Clear()
	{
		SDL_RenderClear(GraphicsContext::Renderer());
	}

	QuadBounds QuadBounds::operator+(const float& value) const
	{
		return { left + value, right + value, bottom + value, top + value };
	}
}

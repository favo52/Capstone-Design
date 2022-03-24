#include "pch.h"
#include "Chesster/Renderer/Renderer.h"

#include "Chesster/Renderer/RenderCommand.h"

namespace Chesster
{
	void Renderer::DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color)
	{
		RenderCommand::SetClearColor(color);
		SDL_RenderFillRect(GraphicsContext::Renderer, &rect);
	}

	void Renderer::DrawTexture(const Texture* texture)
	{
		SDL_RenderCopy(GraphicsContext::Renderer, texture->GetSDLTexture(), texture->m_Clip, &texture->GetBounds());
	}

	void Renderer::DrawTextureEx(const Texture* texture)
	{
		SDL_RenderCopyEx(GraphicsContext::Renderer, texture->GetSDLTexture(), texture->m_Clip,
			&texture->m_RenderRect, texture->m_Angle, texture->m_Center, texture->m_Flip);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	QuadBounds QuadBounds::operator+(const float& value) const
	{
		return { left + value, right + value, bottom + value, top + value };
	}
}

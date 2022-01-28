#include "pch.h"
#include "Chesster_Unleashed/Renderer/Renderer.h"

#include "Chesster_Unleashed/Renderer/RenderCommand.h"
#include "Chesster_Unleashed/Renderer/Context.h"

namespace Chesster
{
	void Renderer::DrawRect(const SDL_Rect& rect, const glm::vec4& color)
	{
		RenderCommand::SetClearColor(color);
		SDL_RenderDrawRect(Context::Renderer, &rect);
	}

	void Renderer::DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color)
	{
		RenderCommand::SetClearColor(color);
		SDL_RenderFillRect(Context::Renderer, &rect);
	}

	void Renderer::DrawLine(const SDL_Point& start, const SDL_Point& end, const glm::vec4& color)
	{
		RenderCommand::SetClearColor(color);
		SDL_RenderDrawLine(Context::Renderer, start.x, start.y, end.x, end.y);
	}

	void Renderer::DrawTexture(const Texture* texture)
	{
		SDL_RenderCopy(Context::Renderer, texture->GetSDLTexture(), texture->m_Clip, &texture->GetBounds());
	}

	void Renderer::DrawTextureEx(const Texture* texture)
	{
		SDL_RenderCopyEx(Context::Renderer, texture->GetSDLTexture(), texture->m_Clip,
			&texture->m_RenderQuad, texture->m_Angle, texture->m_Center, texture->m_Flip);
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

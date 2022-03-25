#pragma once

#include "Chesster/Renderer/Texture.h"

namespace Chesster
{
	/// <summary>
	/// 
	/// </summary>
	class Framebuffer
	{
	public:
		Framebuffer(uint32_t width, uint32_t height);
		virtual ~Framebuffer() = default;

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);

		SDL_Texture* GetSDLTexture() const { return m_Framebuffer.GetSDLTexture(); }

		const uint32_t& GetWidth() const { return m_Framebuffer.GetWidth(); }
		const uint32_t& GetHeight() const { return m_Framebuffer.GetHeight(); }

	private:
		Texture m_Framebuffer;
	};
}

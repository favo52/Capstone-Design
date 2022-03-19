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
		virtual ~Framebuffer();

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);

		Texture GetTexture() const { return m_Framebuffer; }
		SDL_Texture* GetSDLTexture() const { return m_Framebuffer.GetSDLTexture(); }

		const uint32_t& GetWidth() const { return m_Width; }
		const uint32_t& GetHeight() const { return m_Height; }

	private:
		Texture m_Framebuffer;
		uint32_t m_Width{ 0 }, m_Height{ 0 };
	};
}

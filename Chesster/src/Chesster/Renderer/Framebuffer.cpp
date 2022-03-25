#include "pch.h"
#include "Chesster/Renderer/Framebuffer.h"

//#include "Chesster/Renderer/Renderer.h"
#include "Chesster/Renderer/GraphicsContext.h"

namespace Chesster
{
	static const uint32_t s_MaxFrameBufferSize{ 8192 };

	Framebuffer::Framebuffer(uint32_t width, uint32_t height)
	{
		m_Framebuffer.CreateBlank(width, height);
	}

	void Framebuffer::Bind()
	{
		// Make self render target
		SDL_SetRenderTarget(GraphicsContext::Renderer(), m_Framebuffer);
		//Renderer::SetViewport(0, 0, m_Width, m_Height);
	}

	void Framebuffer::Unbind()
	{
		SDL_SetRenderTarget(GraphicsContext::Renderer(), nullptr);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to ({0}, {1}).", width, height);
			return;
		}

		m_Framebuffer.FreeTexture();
		m_Framebuffer.CreateBlank(width, height);
	}
}

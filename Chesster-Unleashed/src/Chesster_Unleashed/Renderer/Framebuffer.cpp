#include "pch.h"
#include "Chesster_Unleashed/Renderer/Framebuffer.h"

#include "Chesster_Unleashed/Renderer/RenderCommand.h"
#include "Chesster_Unleashed/Renderer/Context.h"

#include <SDL.h>

namespace Chesster
{
	static const uint32_t s_MaxFrameBufferSize{ 8192 };

	Framebuffer::Framebuffer(const FramebufferSpecification& spec) :
		m_Specification{ spec }
	{
		m_Framebuffer.CreateBlank(spec.Width, spec.Height);
	}

	Framebuffer::~Framebuffer()
	{
		m_Framebuffer.FreeTexture();
	}

	void Framebuffer::Bind()
	{
		m_Framebuffer.SetAsRenderTarget();
		RenderCommand::SetViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void Framebuffer::Unbind()
	{
		m_Framebuffer.RemoveAsRenderTarget();
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			CHESSTER_WARN("Attempted to resize framebuffer to ({0}, {1}).", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		m_Framebuffer.FreeTexture();
		m_Framebuffer.CreateBlank(width, height);
	}
}

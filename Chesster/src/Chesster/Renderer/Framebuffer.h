#pragma once

#include "Chesster/Renderer/Texture.h"

namespace Chesster
{
	struct FramebufferSpecification
	{
		uint32_t Width{ 0 }, Height{ 0 };
		uint32_t Samples{ 1 };
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		virtual ~Framebuffer();

		void Bind();
		void Unbind();

		void Resize(uint32_t width, uint32_t height);

		Texture GetTexture() const { return m_Framebuffer; }
		SDL_Texture* GetSDLTexture() const { return m_Framebuffer.GetSDLTexture(); }
		const FramebufferSpecification& GetSpec() const { return m_Specification; };

	private:
		Texture m_Framebuffer;
		FramebufferSpecification m_Specification;
	};
}

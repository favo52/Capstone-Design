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
#include "Chesster/Renderer/Framebuffer.h"

#include "Chesster/Renderer/Renderer.h"
#include "Chesster/Game/Board.h"

namespace Chesster
{
	static const uint32_t s_MaxFrameBufferSize{ 8192 };

	Framebuffer::Framebuffer(uint32_t width, uint32_t height)
	{
		m_Framebuffer = std::make_unique<Texture>();
		m_Framebuffer->CreateBlank(width, height);
	}

	void Framebuffer::Bind()
	{
		// Make self render target
		SDL_SetRenderTarget(Renderer::Get(), *m_Framebuffer);
		Renderer::SetViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
	}

	void Framebuffer::Unbind()
	{
		SDL_SetRenderTarget(Renderer::Get(), nullptr);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			LOG_WARN("Attempted to resize framebuffer to ({0}, {1}).", width, height);
			return;
		}

		m_Framebuffer->FreeTexture();
		m_Framebuffer->CreateBlank(width, height);
	}
}

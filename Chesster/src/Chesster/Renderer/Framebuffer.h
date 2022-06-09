/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
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

#pragma once

#include "Chesster/Renderer/Texture.h"

namespace Chesster
{
	/*	*/
	class Framebuffer
	{
	public:
		/**	Creates a Blank texture to function as a Framebuffer to be used as a rendering target.
		 @param width The width of the Framebuffer.
		 @param height The height of the Framebuffer. */
		Framebuffer(uint32_t width, uint32_t height);

		/*	Sets the Framebuffer as the rendering target. */
		void Bind();

		/*	Removes the Framebuffers as the rendering target. */
		void Unbind();

		/**	Destroys and recreates the Framebuffer with the new dimensions. 
			Must be called after a window resize event.
		 @param width The new width for the Framebuffer.
		 @param height The new height for the Framebuffer. */
		void Resize(uint32_t width, uint32_t height);

		/**	
		 @return */
		SDL_Texture* GetSDLTexture() const { return m_Framebuffer->GetSDLTexture(); }

		/**
		 @return The Framebuffer's width in pixels. */
		const uint32_t& GetWidth() const { return m_Framebuffer->GetWidth(); }

		/**
		 @return The Framebuffer's height in pixels. */
		const uint32_t& GetHeight() const { return m_Framebuffer->GetHeight(); }

	private:
		std::unique_ptr<Texture> m_Framebuffer;
	};
}

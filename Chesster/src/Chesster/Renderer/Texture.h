#pragma once

#include "Chesster/Renderer/Font.h"

namespace Chesster
{
	/*	SDL_Texture wrapper class. */
	class Texture
	{
	public:
		/*	Initializes the Texture to nullptr and zeros. */
		Texture() = default;

		/** Creates a texture with the image at specified path.
		 @param filepath The filepath of the image. */
		Texture(const std::string& filepath);

		/** Creates a text texture with the provided font, text and color.
		 @param font The text's font.
		 @param textureText The text's message.
		 @param color The color of the text. */
		Texture(std::shared_ptr<Font> font, const std::string& textureText, SDL_Color color);

		/*	Frees the SDL_Texture. */
		virtual ~Texture();

		/** Creates a Blank texture to be used as a framebuffer/rendering target.
		 @param width The width of the texture, in pixels.
		 @param height The height of the texture, in pixels.
		 @return True if blank texture was created successfully, false otherwise. */
		bool CreateBlank(int width, int height);

		/*	Deallocates texture. */
		void FreeTexture();

		/** Changes the Texture's top left (x, y) position.
		 @param x The top left x coordinate in pixels.
		 @param y The top left y coordinate in pixels. */
		void SetPosition(int x, int y) { m_RenderRect = { x, y, m_RenderRect.w, m_RenderRect.h }; }

		/** Changes the Texture's width.
		 @param width The new width of the texture in pixels. */
		void SetWidth(int width) { m_RenderRect.w = width; }

		/** Changes the Texture's height.
		 @param height The new height of the texture in pixels. */
		void SetHeight(int height) { m_RenderRect.h = height; }
		
		/** Changes the Texture's height.
		 @param width The new height of the texture in pixels. */
		void SetClip(SDL_Rect* renderClip) { m_RenderClip = renderClip; }

		/** 
		 @return The Texture's width. */
		const int GetWidth() const { return m_RenderRect.w; }

		/**
		 @return The Texture's height. */
		const int GetHeight() const { return m_RenderRect.h; }

		/** The dimensions of the texture.
		 @return The Texture's SDL_Rect member variable. */
		const SDL_Rect& GetBounds() const { return m_RenderRect; }

		/**
		 @return A pointer of the Texture's rendering clip. */
		const SDL_Rect* GetRenderClip() const { return m_RenderClip; }

		/** The actual hardware texture.
		 @return A pointer to the Texture's SDL_Texture member variable. */
		SDL_Texture* GetSDLTexture() const { return m_Texture; }

		operator SDL_Texture*() const { return m_Texture; }

	private:
		SDL_Texture* m_Texture{ nullptr };		// The actual hardware texture

		SDL_Rect m_RenderRect{ 0, 0, 0, 0 };	// The dimensions of the texture
		SDL_Rect* m_RenderClip{ nullptr };		// The SDL_Texture clip
	};
}

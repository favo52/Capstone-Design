#pragma once

#include "Chesster/Renderer/Font.h"

namespace Chesster
{
	/// <summary>
	/// Texture wrapper class.
	/// </summary>
	class Texture
	{
	public:
		/// <summary>
		/// Initializes the Texture to nullptr and zeros.
		/// </summary>
		Texture() = default;

		/// <summary>
		/// Creates a texture with the image at specified path.
		/// </summary>
		/// <param name="path">The filepath of the image.</param>
		Texture(const std::string& path);

		/// <summary>
		/// Creates a text texture with the provided font, text and color.
		/// </summary>
		/// <param name="font">The text's font.</param>
		/// <param name="textureText">The text's message.</param>
		/// <param name="color">The color of the text.</param>
		Texture(std::shared_ptr<Font> font, const std::string& textureText, SDL_Color color);

		/// <summary>
		/// 
		/// </summary>
		virtual ~Texture();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <returns>True if blank texture was created successfully, False otherwise.</returns>
		bool CreateBlank(int width, int height);

		/// <summary>
		/// Deallocates texture.
		/// </summary>		
		void FreeTexture();

		void SetPosition(int x, int y) { m_RenderRect = { x, y, m_RenderRect.w, m_RenderRect.h }; }
		void SetWidth(int width) { m_RenderRect.w = width; }
		void SetHeight(int height) { m_RenderRect.h = height; }
		void SetClip(SDL_Rect* renderClip) { m_RenderClip = renderClip; }

		// Get image dimensions
		const int GetWidth() const { return m_RenderRect.w; }
		const int GetHeight() const { return m_RenderRect.h; }

		const SDL_Rect& GetBounds() const { return m_RenderRect; }

		const SDL_Rect* GetRenderClip() const { return m_RenderClip; }

		SDL_Texture* GetSDLTexture() const { return m_Texture; }
		operator SDL_Texture*() const { return m_Texture; }

	private:
		SDL_Texture* m_Texture{ nullptr };		// The actual hardware texture

		SDL_Rect m_RenderRect{ 0, 0, 0, 0 };	// The dimensions of the texture
		SDL_Rect* m_RenderClip{ nullptr };		// The SDL_Texture clip
	};
}

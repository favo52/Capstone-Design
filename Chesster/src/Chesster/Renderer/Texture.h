#pragma once

#include "Chesster/Renderer/Font.h"

#include <SDL_render.h>

namespace Chesster
{
	/// <summary>
	/// Texture wrapper class.
	/// </summary>
	class Texture
	{
	public:
		Texture() = default;

		/// <summary>
		/// Loads image at specified path.
		/// </summary>
		/// <param name="path">The filepath of the image.</param>
		/// <returns>True if image loaded successfully, False otherwise.</returns>
		Texture(const std::string& path);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="font"></param>
		/// <param name="textureText"></param>
		/// <param name="color"></param>
		Texture(std::shared_ptr<Font> font, const std::string& textureText, SDL_Color color);

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

		void SetPosition(int x, int y) { m_RenderRect = { x, y, m_Width, m_Height }; }
		void SetWidth(int width) { m_Width = width; }
		void SetHeight(int height) { m_Height = height; }
		void SetClip(SDL_Rect* clip) { m_Clip = clip; }
		void SetRotation(float angle) { m_Angle = angle; }
		void SetCenter(SDL_Point* center) { m_Center = center; }
		void SetFlip(SDL_RendererFlip flip) { m_Flip = flip; }

		void SetAsRenderTarget();
		void RemoveAsRenderTarget();

		// Get image dimensions
		const int GetWidth() const { return m_Width; }
		const int GetHeight() const { return m_Height; }

		const SDL_Rect& GetBounds() const { return m_RenderRect; }
		SDL_Rect& GetBounds() { return m_RenderRect; }

		SDL_Texture* GetSDLTexture() const { return m_Texture; }

		operator SDL_Texture*() const { return m_Texture; }

	private:
		SDL_Texture* m_Texture{ nullptr }; // The actual hardware texture
		int m_Width{ 0 }, m_Height{ 0 };

		SDL_Rect m_RenderRect{ 0, 0, 0, 0 };
		float m_Angle{ 0.0f };
		SDL_Rect* m_Clip{ nullptr };
		SDL_Point* m_Center{ nullptr };
		SDL_RendererFlip m_Flip{ SDL_FLIP_NONE };

		friend class Renderer; // Allows access to private
	};
}

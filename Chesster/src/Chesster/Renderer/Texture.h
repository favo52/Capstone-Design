#pragma once

#include <SDL_render.h>
#include <SDL_ttf.h>

namespace Chesster
{
	/// <summary>
	/// Abstraction of the SDL_ttf addon.
	/// Loads fonts from ttf files. Closes fonts when not wanted anymore.
	/// </summary>
	struct Font
	{
		TTF_Font* m_Font{ nullptr }; // Holds the font information.

		/// <summary>
		/// Loads a ttf file.
		/// </summary>
		/// <param name="filename">The filepath of the ttf file.</param>
		/// <param name="size">The size to be set to the font.</param>
		/// <returns>True if font loaded successfully, False otherwise.</returns>
		bool LoadFromFile(const std::string& filename, int size = 30)
		{
			m_Font = TTF_OpenFont(filename.c_str(), size);
			if (m_Font == nullptr)
			{
				LOG_ERROR("Failed to load '{0}'! SDL_ttf error: {1}", filename, TTF_GetError());
				return false;
			}
			return true;
		}

		/// <summary>
		/// Releases all the font's resources.
		/// </summary>
		void CloseFont()
		{
			if (m_Font != nullptr)
				TTF_CloseFont(m_Font);
		}
	};

	/// <summary>
	/// Texture wrapper class.
	/// </summary>
	class Texture
	{
	public:
		Texture() = default;
		Texture(const Texture&) = default;
		Texture(const std::string& path);
		virtual ~Texture();

		/// <summary>
		/// Loads image at specified path.
		/// </summary>
		/// <param name="path">The filepath of the image.</param>
		/// <returns>True if image loaded successfully, False otherwise.</returns>
		bool LoadFromFile(const std::string& path);
		bool LoadFromRenderedText(Font font, const std::string& textureText, SDL_Color color);

		/// <summary>
		/// Creates a blank texture.
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name=""></param>
		/// <returns>True if blank texture was created successfully, False otherwise.</returns>
		bool CreateBlank(int width, int height, SDL_TextureAccess access = SDL_TEXTUREACCESS_TARGET);

		/// <summary>
		/// Deallocates texture.
		/// </summary>		
		void FreeTexture();

		/// <summary>
		/// Sets the texture's color modulation. It uses RGB values.
		/// </summary>
		/// <param name="red">The red value of the color.</param>
		/// <param name="green">The green value of the color.</param>
		/// <param name="blue">The blue value of the color.</param>
		void SetColor(Uint8 red, Uint8 green, Uint8 blue);

		/// <summary>
		/// Sets blending. See SDL documentation for blending modes.
		/// </summary>
		/// <param name="blending">The blending mode.</param>
		void SetBlendMode(SDL_BlendMode blending);

		/// <summary>
		/// Sets the alpha modulation. It affects the opacity.
		/// </summary>
		/// <param name="alpha">The alpha value.</param>
		void SetAlpha(Uint8 alpha);

		void SetPosition(int x, int y) { m_RenderQuad = { x, y, m_Width, m_Height }; }
		void SetWidth(int width) { m_Width = width; }
		void SetHeight(int height) { m_Height = height; }
		void SetClip(SDL_Rect* clip) { m_Clip = clip; }
		void SetRotation(double angle) { m_Angle = angle; }
		void SetCenter(SDL_Point* center) { m_Center = center; }
		void SetFlip(SDL_RendererFlip flip) { m_Flip = flip; }

		void SetAsRenderTarget();
		void RemoveAsRenderTarget();

		// Get image dimensions
		const int GetWidth() const { return m_Width; }
		const int GetHeight() const { return m_Height; }
		const SDL_Rect& GetBounds() const { return m_RenderQuad; }
		SDL_Rect& GetBounds() { return m_RenderQuad; }

		// Pixel manipulators
		bool LockTexture();
		bool UnlockTexture();

		void CopyPixels(void* pixels);
		Uint32 GetPixel32(const unsigned int& x, const unsigned int& y);

		void* GetPixels() { return m_Pixels; }
		int GetPitch() const { return m_Pitch; }

		SDL_Texture* GetSDLTexture() const { return m_Texture; }

		operator SDL_Texture*() const { return m_Texture; }

	private:
		SDL_Texture* m_Texture{ nullptr }; // The actual hardware texture
		int m_Width{ 0 }, m_Height{ 0 };
		void* m_Pixels{ nullptr };
		int m_Pitch{ 0 };

		SDL_Rect m_RenderQuad{ 0, 0, 0, 0 };
		double m_Angle{ 0.0 };
		SDL_Rect* m_Clip{ nullptr };
		SDL_Point* m_Center{ nullptr };
		SDL_RendererFlip m_Flip{ SDL_FLIP_NONE };

		friend class Renderer; // Allows access to private
	};
}

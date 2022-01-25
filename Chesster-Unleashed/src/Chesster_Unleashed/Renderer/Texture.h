#pragma once

#include <SDL_render.h>
#include <SDL_ttf.h>

namespace Chesster
{
	struct Font
	{
		TTF_Font* m_Font{ nullptr };

		bool LoadFromFile(const std::string& filename, const int& size = 30)
		{
			m_Font = TTF_OpenFont(filename.c_str(), size);
			if (m_Font == nullptr)
			{
				CHESSTER_ERROR("Failed to load '{0}'! SDL_ttf error: {1}", filename, TTF_GetError());
				return false;
			}
			return true;
		}

		void CloseFont()
		{
			if (m_Font != nullptr)
				TTF_CloseFont(m_Font);
		}
	};

	class Texture
	{
	public:
		Texture() = default;
		Texture(const std::string& path);
		virtual ~Texture();

		bool LoadFromFile(const std::string& path);
		bool LoadFromRenderedText(Font font, const std::string& textureText, SDL_Color color);

		bool CreateBlank(int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_TARGET);

		// Deallocates texture
		void FreeTexture();

		// Change properties
		void SetColor(Uint8 red, Uint8 green, Uint8 blue);
		void SetBlendMode(SDL_BlendMode blending);
		void SetAlpha(Uint8 alpha);

		void SetPosition(int x, int y) { m_RenderQuad = { x, y, m_Width, m_Height }; }
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
		SDL_Texture* m_Texture{ nullptr };
		int m_Width{ 0 }, m_Height{ 0 };
		void* m_Pixels{ nullptr };
		int m_Pitch{ 0 };

		SDL_Rect m_RenderQuad{ 0, 0, 0, 0 };
		double m_Angle{ 0.0 };
		SDL_Rect* m_Clip{ nullptr };
		SDL_Point* m_Center{ nullptr };
		SDL_RendererFlip m_Flip{ SDL_FLIP_NONE };

		friend class Renderer;
	};
}

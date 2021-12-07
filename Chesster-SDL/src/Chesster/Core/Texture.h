#pragma once

#include "pch.h"
#include "Vector.h"

#include "SDL_ttf.h"

namespace Chesster
{
	enum class TextureID
	{
		Board,
		Pieces,
		ChessterLogo,
		ReadySetCode
	};

	enum class FontID
	{
		AbsEmpire_100,
		OpenSans,
		OpenSans_100,
		Minecraft,
		Minecraft_10,
		Minecraft_100,
		Sansation,
		Sansation_10,
		Sansation_100
	};

	struct Font // To allow font ResourceHolder
	{
		bool LoadFromFile(const std::string& filename, const int& size = 30)
		{
			m_Font = TTF_OpenFont(filename.c_str(), size);
			if (m_Font == nullptr)
			{
				CHESSTER_ERROR("Failed to load {0}! SDL_ttf error: {1}", filename, TTF_GetError());
				return false;
			}
			return true;
		}

		TTF_Font* m_Font;
	};

	class Texture
	{
	public:
		Texture();
		~Texture();

		bool LoadFromFile(const std::string& path);
		bool LoadFromRenderedText(Font font, const std::string& textureText, SDL_Color color);
		
		void FreeTexture();

		// Change properties
		void SetColor(Uint8 red, Uint8 green, Uint8 blue);
		void SetBlendMode(SDL_BlendMode blending);
		void SetAlpha(Uint8 alpha);

		void SetPosition(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0,
			SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

		void Move(const Vector2f& offset);
		void Move(const double& offsetX, const double& offsetY);

		// Get properties
		inline const Vector2f GetPosition() const { return Vector2f(m_RenderQuad.x, m_RenderQuad.y); }
		inline Vector2f GetPosition() { return Vector2f(m_RenderQuad.x, m_RenderQuad.y); }

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }
		inline const SDL_Rect GetBounds() const { return m_RenderQuad; }
		
		void Draw() const;

	private:
		// Image Properties
		SDL_Texture* m_Texture;
		int m_Width;
		int m_Height;

		// Render Properties
		SDL_Rect m_RenderQuad;
		SDL_Rect* m_Clip;
		double m_Angle;
		SDL_Point* m_Center;
		SDL_RendererFlip m_Flip;
	};
}

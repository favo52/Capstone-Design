#pragma once

#include "pch.h"
#include "Window.h"

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
		Minecraft,
		Minecraft_100,
		Sansation,
		Sansation_10,
		Sansation_100
	};

	struct Vector2f
	{
		float x = 0.0f, y = 0.0f;
	};

	class Texture
	{
	public:
		struct Font // To allow font ResourceHolder
		{
			bool LoadFromFile(const std::string& filename, const int& size = 30)
			{
				m_fMinecraft = TTF_OpenFont(filename.c_str(), size);
				if (m_fMinecraft == nullptr)
				{
					std::cout << "Failed to load " << filename << "! SDL_ttf Error: " << TTF_GetError() << '\n';
					return false;
				}
				return true;
			}
			
			TTF_Font* m_fMinecraft;
		};

	public:
		Texture();
		~Texture();

		bool LoadFromFile(const std::string& path);
		bool LoadFromRenderedText(Font font, const std::string& textureText, SDL_Color color);
		
		void FreeTexture();

		void SetColor(Uint8 red, Uint8 green, Uint8 blue);
		void SetBlendMode(SDL_BlendMode blending);
		void SetAlpha(Uint8 alpha);

		//void SetWidth(const int& width) { m_Width = width; }
		//void SetHeight(const int& height) { m_Height = height; }

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		//inline void SetTexture(SDL_Texture* texture) { m_Texture = texture; }
		//inline SDL_Texture* GetTexture() { return m_Texture; }

		void SetPosition(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0,
			SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

		void Draw() const;

	private:
		SDL_Texture* m_Texture;
		int m_Width;
		int m_Height;

		SDL_Rect m_RenderQuad;
		SDL_Rect* m_Clip;
		double m_Angle;
		SDL_Point* m_Center;
		SDL_RendererFlip m_Flip;
	};
}

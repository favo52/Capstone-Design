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

	struct Vector2f;

	struct Vector2i
	{
		Vector2i(int a = 0, int b = 0)
		{
			x = a;
			y = b;
		}

		Vector2i operator-=(const Vector2i& vec2i)
		{
			x = x - vec2i.x;
			y = y - vec2i.y;
			return Vector2i(this->x, this->y);
		}

		Vector2i operator+(const SDL_Point& vec2f) { return Vector2i(this->x + vec2f.x, this->y + vec2f.y); }

		int x = 0, y = 0;
	};

	struct Vector2f
	{
		Vector2f(float a = 0.0f, float b = 0.0f)
		{
			x = a;
			y = b;
		}

		Vector2f operator-() { return Vector2f(x * -1, y * -1); }
		Vector2f operator+(const Vector2f& vec2f) { return Vector2f(this->x + vec2f.x, this->y + vec2f.y); }
		Vector2f operator+(const SDL_Point& vec2f) { return Vector2f(this->x + vec2f.x, this->y + vec2f.y); }
		
		Vector2f operator*(const float& f) { return Vector2f(x * f, y * f); }
		Vector2f operator/(const float& f) { return Vector2f(x / f, y / f); }

		bool operator==(const Vector2f& vec2f) { return x == vec2f.x && y == vec2f.y; }
		bool operator!=(const Vector2f& vec2f) { return !(*this == vec2f); }

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

		void Move(const Vector2f& offset);

		void SetPosition(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0,
			SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

		inline const Vector2f GetPosition() const { return Vector2f(m_RenderQuad.x, m_RenderQuad.y); }
		inline Vector2f GetPosition() { return Vector2f(m_RenderQuad.x, m_RenderQuad.y); }
		
		/*inline const SDL_Point GetPosition() const
		{
			SDL_Point point = { m_RenderQuad.x, m_RenderQuad.y };
			return point;
		}

		inline SDL_Point GetPosition()
		{
			SDL_Point point = { m_RenderQuad.x, m_RenderQuad.y };
			return point;
		}*/

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }
		inline const SDL_Rect GetBounds() const { return m_RenderQuad; }
		
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
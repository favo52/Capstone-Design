#include "pch.h"
#include "Texture.h"
#include "Window.h"

namespace Chesster
{
	Texture::Texture() :
		m_Texture{ nullptr },
		m_Width{ 0 },
		m_Height{ 0 },
		//m_Acceleration{ 0.0 },
		//m_Velocity{ 0.0 },
		m_RenderQuad{ 0, 0, m_Width, m_Height },
		m_Clip{ nullptr },
		m_Angle{ 0.0 },
		m_Center{ nullptr },
		m_Flip{ SDL_FLIP_NONE }
	{
	}

	Texture::~Texture()
	{
		FreeTexture();
	}

	bool Texture::LoadFromFile(const std::string& path)
	{
		// Get rid of preexisting texture
		FreeTexture();

		// The final texture
		SDL_Texture* newTexture = nullptr;

		// Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == nullptr)
		{
			CHESSTER_ERROR("Unable to load image {0}! SDL_image error: {1}", path.c_str(), IMG_GetError());
		}
		else
		{
			// Color key image. Makes specified color transparent.
			//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

			// Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(Window::Renderer, loadedSurface);
			if (newTexture == nullptr)
			{
				CHESSTER_ERROR("Unable to create texture from {0}! SDL error: {1}", path.c_str(), SDL_GetError());
			}
			else
			{
				// Get image dimensions
				m_Width = loadedSurface->w;
				m_Height = loadedSurface->h;
			}

			// Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		m_Texture = newTexture;

		return m_Texture != nullptr;
	}

	bool Texture::LoadFromRenderedText(Font font, const std::string& textureText, SDL_Color color)
	{
		// Get rid of preexisting texture
		FreeTexture();
		
		// Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(font.m_Font, textureText.c_str(), color);
		if (textSurface == nullptr)
		{
			CHESSTER_ERROR("Unable to render text surface! SDL_ttf error: {0}", TTF_GetError());
		}
		else
		{
			// Create texture from surface pixels
			m_Texture = SDL_CreateTextureFromSurface(Window::Renderer, textSurface);
			if (m_Texture == nullptr)
			{
				CHESSTER_ERROR("Unable to create texture from rendered text! SDL error: {0}", SDL_GetError());
			}
			else
			{
				// Get image dimensions
				m_Width = textSurface->w;
				m_Height = textSurface->h;
			}

			// Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		// Return success
		return m_Texture != nullptr;
	}

	void Texture::FreeTexture()
	{
		// Free texture if it exists
		if (m_Texture != nullptr)
		{
			SDL_DestroyTexture(m_Texture);
			m_Texture = nullptr;
			m_Width = 0;
			m_Height = 0;
		}
	}

	void Texture::SetColor(Uint8 red, Uint8 green, Uint8 blue)
	{
		// Modulate texture rgb
		SDL_SetTextureColorMod(m_Texture, red, green, blue);
	}

	void Texture::SetBlendMode(SDL_BlendMode blending)
	{
		// Set blending function
		SDL_SetTextureBlendMode(m_Texture, blending);
	}

	void Texture::SetAlpha(Uint8 alpha)
	{
		// Modulate texture alpha
		SDL_SetTextureAlphaMod(m_Texture, alpha);
	}

	void Texture::SetPosition(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
	{
		// Set rendering space
		m_RenderQuad = { x, y, m_Width, m_Height };

		m_Clip = clip;
		m_Angle = angle;
		m_Center = center;
		m_Flip = flip;

		// Set clip rendering dimensions
		if (clip != nullptr)
		{
			m_RenderQuad.w = clip->w;
			m_RenderQuad.h = clip->h;
		}
	}

	/*void Texture::SetVelocity(Vector2f velocity)
	{
		m_Velocity = velocity;
	}

	void Texture::SetVelocity(double vx, double vy)
	{
		m_Velocity.x = vx;
		m_Velocity.y = vy;
	}

	void Texture::Accelerate(Vector2f acceleration)
	{
		m_Velocity.x += acceleration.x;
		m_Velocity.y += acceleration.y;
	}

	void Texture::Accelerate(double ax, double ay)
	{
		m_Velocity.x += ax;
		m_Velocity.y += ay;
	}*/

	void Texture::Move(const Vector2f& offset)
	{
		m_RenderQuad.x += offset.x;
		m_RenderQuad.y += offset.y;
	}

	void Texture::Move(const double& offsetX, const double& offsetY)
	{
		m_RenderQuad.x += offsetX;
		m_RenderQuad.y += offsetY;
	}

	void Texture::Draw() const
	{
		// Render to screen
		SDL_RenderCopyEx(Window::Renderer, m_Texture, m_Clip, &m_RenderQuad, m_Angle, m_Center, m_Flip);
	}
}

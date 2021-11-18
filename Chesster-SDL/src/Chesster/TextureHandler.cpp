#include "pch.h"
#include "TextureHandler.h"

namespace Chesster
{
	Texture::Texture() :
		m_Texture{ nullptr },
		m_Width{ 0 },
		m_Height{ 0 }
	{
	}

	Texture::~Texture()
	{
		FreeTexture();
	}

	void Texture::Render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
	{

	}

	void Texture::FreeTexture()
	{
		//Free texture if it exists
		if (m_Texture != NULL)
		{
			SDL_DestroyTexture(m_Texture);
			m_Texture = NULL;
			m_Width = 0;
			m_Height = 0;
		}
	}

	void Texture::SetColor(Uint8 red, Uint8 green, Uint8 blue)
	{
		//Modulate texture rgb
		SDL_SetTextureColorMod(m_Texture, red, green, blue);
	}

	void Texture::SetBlendMode(SDL_BlendMode blending)
	{
		//Set blending function
		SDL_SetTextureBlendMode(m_Texture, blending);
	}

	void Texture::SetAlpha(Uint8 alpha)
	{
		//Modulate texture alpha
		SDL_SetTextureAlphaMod(m_Texture, alpha);
	}
}

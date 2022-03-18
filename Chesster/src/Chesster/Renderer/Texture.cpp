#include "pch.h"
#include "Chesster/Renderer/Texture.h"

#include "Chesster/Renderer/GraphicsContext.h"

#include <SDL_image.h>

namespace Chesster
{
	Texture::Texture(const std::string& path)
	{
		LoadFromFile(path);
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
		SDL_Texture* newTexture{ nullptr };

		// Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == nullptr)
		{
			LOG_ERROR("Unable to load image '{0}'! SDL_image Error: {1}", path.c_str(), IMG_GetError());
		}
		else
		{
			// Convert surface to display format
			SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
			if (formattedSurface == nullptr)
			{
				LOG_ERROR("Unable to convert loaded surface to display format! SDL Error: {0}", SDL_GetError());
			}
			else
			{
				// Create blank streamable texture
				newTexture = SDL_CreateTexture(GraphicsContext::Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
				if (newTexture == nullptr)
				{
					LOG_ERROR("Unable to create blank texture! SDL Error: {0}", SDL_GetError());
				}
				else
				{
					// Enable blending on texture
					SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

					// Lock texture for manipulation
					SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &m_Pixels, &m_Pitch);

					// Copy loaded/formatted surface pixels
					memcpy(m_Pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
					
					// Get image dimensions
					m_Width = formattedSurface->w;
					m_Height = formattedSurface->h;

					// Get pixel data in editable format
					Uint32* pixels = (Uint32*)m_Pixels;
					int pixelCount = (m_Pitch / 4) * m_Height;

					// Map colors				
					Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
					Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

					// Color key pixels
					for (int i = 0; i < pixelCount; ++i)
					{
						if (pixels[i] == colorKey)
						{
							pixels[i] = transparent;
						}
					}

					// Unlock texture to update
					SDL_UnlockTexture(newTexture);
					m_Pixels = NULL;
				}

				// Get rid of old formatted surface
				SDL_FreeSurface(formattedSurface);
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
			LOG_ERROR("Unable to render text surface! SDL_ttf Error: {0}", TTF_GetError());
		}
		else
		{
			// Create texture from surface pixels
			m_Texture = SDL_CreateTextureFromSurface(GraphicsContext::Renderer, textSurface);
			if (m_Texture == nullptr)
			{
				LOG_ERROR("Unable to create texture from rendered text! SDL Error: {0}", SDL_GetError());
			}
			else
			{
				// Get image dimensions
				m_Width = textSurface->w;
				m_Height = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		// Return success
		return m_Texture != nullptr;
	}

	bool Texture::CreateBlank(int width, int height, SDL_TextureAccess access)
	{
		// Create uninitialized texture
		m_Texture = SDL_CreateTexture(GraphicsContext::Renderer, SDL_PIXELFORMAT_RGBA32, access, width, height);
		if (m_Texture == nullptr)
		{
			LOG_ERROR("Unable to create blank texture! SDL Error: {0}", SDL_GetError());
		}
		else
		{
			m_Width = width;
			m_Height = height;
		}

		return m_Texture != nullptr;
	}

	void Texture::FreeTexture()
	{
		// Free texture if it exists
		if (m_Texture != nullptr)
		{
			SDL_DestroyTexture(m_Texture);
			m_Texture = nullptr;
			m_Pixels = nullptr;
			m_Pitch = 0;
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

	void Texture::SetAsRenderTarget()
	{
		// Make self render target
		SDL_SetRenderTarget(GraphicsContext::Renderer, m_Texture);
	}

	void Texture::RemoveAsRenderTarget()
	{
		SDL_SetRenderTarget(GraphicsContext::Renderer, nullptr);
	}

	bool Texture::LockTexture()
	{
		bool success{ true };

		// Texture is already locked
		if (m_Pixels != nullptr)
		{
			LOG_WARN("Texture is already locked!");
			success = false;
		}
		// Lock texture
		else
		{
			if (SDL_LockTexture(m_Texture, nullptr, &m_Pixels, &m_Pitch) != 0)
			{
				LOG_ERROR("Unable to lock texture! SDL Error: {0}", SDL_GetError());
				success = false;
			}
		}

		return success;
	}

	bool Texture::UnlockTexture()
	{
		bool success{ true };

		// Texture is not locked
		if (m_Pixels == nullptr)
		{
			LOG_WARN("Texture is not locked!");
			success = false;
		}
		// Unlock texture
		else
		{
			SDL_UnlockTexture(m_Texture);
			m_Pixels = nullptr;
			m_Pitch = 0;
		}

		return success;
	}

	void Texture::CopyPixels(void* pixels)
	{
		// Texture is locked
		if (m_Pixels != nullptr)
		{
			// Copy to locked pixels
			memcpy(m_Pixels, pixels, m_Pitch * m_Height);
		}
	}

	Uint32 Texture::GetPixel32(const unsigned int& x, const unsigned int& y)
	{
		// Convert the pixels to 32 bit
		Uint32* pixels = (Uint32*)m_Pixels;

		// Get the pixel requested
		return pixels[(y * (m_Pitch / 4)) + x];
	}
}

#include "pch.h"
#include "Chesster/Renderer/Texture.h"

#include "Chesster/Renderer/GraphicsContext.h"

#include <SDL_image.h>

namespace Chesster
{
	// Disguises std::runtime_error()
	inline void RuntimeError(const std::string& s) { throw std::runtime_error(s); }

	Texture::Texture(const std::string& path)
	{
		const std::string errorMsg{ "Failed to load image! Check Chesster.log for more information." };

		// Get rid of preexisting texture
		FreeTexture();

		// Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == nullptr)
		{
			LOG_ERROR("Unable to load image '{0}'! SDL_image error: {1}", path.c_str(), IMG_GetError());
			RuntimeError(errorMsg);
		}

		// Convert surface to display format
		SDL_Surface* formattedSurface{ nullptr };
		formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		if (formattedSurface == nullptr)
		{
			LOG_ERROR("Unable to convert loaded surface to display format! SDL error: {0}", SDL_GetError());
			RuntimeError(errorMsg);
		}
		else
		{
			// Create blank streamable texture
			SDL_Texture* texture{ nullptr };
			texture = SDL_CreateTexture(GraphicsContext::Renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
			if (texture == nullptr)
			{
				LOG_ERROR("Unable to create blank texture! SDL Error: {0}", SDL_GetError());
				RuntimeError(errorMsg);
			}

			// Enable blending on texture
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

			// Lock texture for manipulation
			void* pixelData{ nullptr };
			int pitch{ 0 };
			SDL_LockTexture(texture, &formattedSurface->clip_rect, &pixelData, &pitch);

			// Copy loaded/formatted surface pixels
			memcpy(pixelData, formattedSurface->pixels, static_cast<size_t>(formattedSurface->pitch) * formattedSurface->h);

			// Get image dimensions
			m_RenderRect = { 0, 0, formattedSurface->w, formattedSurface->h };

			// Unlock texture to update
			SDL_UnlockTexture(texture);

			m_Texture = texture;
		}

		// Get rid of temporary surfaces
		SDL_FreeSurface(formattedSurface);
		SDL_FreeSurface(loadedSurface);
	}

	Texture::Texture(std::shared_ptr<Font> font, const std::string& textureText, SDL_Color color)
	{
		// Get rid of preexisting texture
		FreeTexture();

		// Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(font->GetTTF(), textureText.c_str(), color);
		if (textSurface == nullptr)
		{
			LOG_ERROR("Unable to render text surface! SDL_ttf error: {0}", TTF_GetError());
			RuntimeError("Unable to create load text! Verify Chesster.log for more info.");
		}
		else
		{
			// Create texture from surface pixels
			m_Texture = SDL_CreateTextureFromSurface(GraphicsContext::Renderer(), textSurface);
			if (m_Texture == nullptr)
			{
				SDL_FreeSurface(textSurface);
				LOG_ERROR("Unable to create texture from rendered text! SDL error: {0}", SDL_GetError());
				RuntimeError("Unable to create load text! Verify Chesster.log for more info.");
			}
			else
			{
				// Get image dimensions
				m_RenderRect.w = textSurface->w;
				m_RenderRect.h = textSurface->h;
			}

			// Get rid of old surface
			SDL_FreeSurface(textSurface);
		}
	}

	Texture::~Texture()
	{
		FreeTexture();
	}

	bool Texture::CreateBlank(int width, int height)
	{
		// Create uninitialized texture
		m_Texture = SDL_CreateTexture(GraphicsContext::Renderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
		if (m_Texture == nullptr)
		{
			LOG_ERROR("Unable to create blank texture! SDL Error: {0}", SDL_GetError());
		}
		else
		{
			m_RenderRect.w = width;
			m_RenderRect.h = height;
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
			m_RenderRect = { 0, 0, 0, 0 };
			m_RenderClip = nullptr;
		}
	}
}

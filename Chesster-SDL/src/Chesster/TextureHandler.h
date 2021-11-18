#pragma once

#include "pch.h"
#include "Window.h"

namespace Chesster
{
	enum class TexturesID
	{
		Board,
		Pieces,
		ChessterLogo,
		ReadySetCode
	};

	enum class FontID
	{
		Minecraft
	};

	class Texture
	{
	public:
		Texture();
		~Texture();

		void Render(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0,
			SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
		
		void FreeTexture();

		void SetColor(Uint8 red, Uint8 green, Uint8 blue);
		void SetBlendMode(SDL_BlendMode blending);
		void SetAlpha(Uint8 alpha);

		void SetWidth(const int& width) { m_Width = width; }
		void SetHeight(const int& height) { m_Height = height; }

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		inline void SetTexture(SDL_Texture* texture) { m_Texture = texture; }
		inline SDL_Texture* GetTexture() { return m_Texture; }

	private:
		SDL_Texture* m_Texture;
		int m_Width;
		int m_Height;
	};

	/*****************************************************
	*	TextureHandler
	*****************************************************/

	template<typename Identifier>
	class TextureHandler
	{
	public:
		void LoadFromFile(Identifier id, const std::string& path);
		void LoadFromRenderedText(Identifier id, const std::string& textTexture, SDL_Color textColor);

		Texture& Get(Identifier id);
		const Texture& Get(Identifier id) const;

	private:
		void InsertResource(Identifier id, std::unique_ptr<Texture> resource);

	private:
		Texture m_Texture;
		std::unordered_map<Identifier, std::unique_ptr<Texture>> m_ResourceMap;
	};
	
	/*****************************************************
	*	TextureHandler Implementation
	*****************************************************/

	template<typename Identifier>
	inline void TextureHandler<Identifier>::LoadFromFile(Identifier id, const std::string& path)
	{
		std::unique_ptr<Texture> newTexture(new Texture());

		// Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == nullptr)
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		else
		{
			// Color key image
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

			// Create texture from surface pixels
			newTexture->SetTexture(SDL_CreateTextureFromSurface(Window::Renderer, loadedSurface));
			if (newTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}
			else
			{
				// Get image dimensions
				newTexture->SetWidth(loadedSurface->w);
				newTexture->SetHeight(loadedSurface->h);
			}

			// Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		// If loading successful, insert resource to map
		InsertResource(id, std::move(newTexture));
	}

	template<typename Identifier>
	inline void TextureHandler<Identifier>::LoadFromRenderedText(Identifier id, const std::string& textTexture, SDL_Color textColor)
	{

	}

	template<typename Identifier>
	inline Texture& TextureHandler<Identifier>::Get(Identifier id)
	{
		auto found = m_ResourceMap.find(id);
		assert(found != m_ResourceMap.end());

		return *found->second;
	}

	template<typename Identifier>
	inline const Texture& TextureHandler<Identifier>::Get(Identifier id) const
	{
		auto found = m_ResourceMap.find(id);
		assert(found != m_ResourceMap.end());

		return *found->second;
	}

	template<typename Identifier>
	inline void TextureHandler<Identifier>::InsertResource(Identifier id, std::unique_ptr<Texture> resource)
	{
		// Insert and check success
		auto inserted = m_ResourceMap.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}
}

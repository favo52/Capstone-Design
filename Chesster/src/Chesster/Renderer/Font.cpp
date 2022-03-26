#include "pch.h"
#include "Chesster/Renderer/Font.h"

namespace Chesster
{
	Font::Font(const std::string& filename, int size) :
		m_Font{ nullptr }
	{
		m_Font = TTF_OpenFont(filename.c_str(), size);
		if (m_Font == nullptr)
		{
			LOG_ERROR("Failed to load '{0}'! SDL_ttf error: {1}", filename, TTF_GetError());
			throw std::runtime_error("Error loading font! Please check Chesster.log for more information.");
		}
	}

	Font::~Font()
	{
		CloseFont();
	}

	void Font::CloseFont()
	{
		if (m_Font != nullptr)
		{
			TTF_CloseFont(m_Font);
			m_Font = nullptr;
		}
	}
}

#pragma once

#include <SDL_ttf.h>

namespace Chesster
{
	/*	Abstraction of the SDL_ttf addon.
		Loads fonts from ttf files. Closes fonts when not wanted anymore. */
	class Font
	{
	public:
		/** Loads a ttf file.
		 @param filename The filepath of the ttf file.
		 @param size The size to be set to the font. */
		Font(const std::string& filename, int size = 30);

		/*	Calls CloseFont() */
		virtual ~Font();

		/*	Releases all the font's resources. */
		void CloseFont();

		/** Holds the font information.
		 @return A pointer to the Font's TTF_Font member variable. */
		TTF_Font* GetTTF() const { return m_Font; }

	private:
		TTF_Font* m_Font; // Holds the font information.
	};
}

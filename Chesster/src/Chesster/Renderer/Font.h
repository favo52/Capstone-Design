#pragma once

#include <SDL_ttf.h>

namespace Chesster
{
	/// <summary>
	/// Abstraction of the SDL_ttf addon.
	/// Loads fonts from ttf files. Closes fonts when not wanted anymore.
	/// </summary>
	class Font
	{
	public:
		Font(const std::string& filename, int size = 30);

		/// <summary>
		/// Loads a ttf file.
		/// </summary>
		/// <param name="filename">The filepath of the ttf file.</param>
		/// <param name="size">The size to be set to the font.</param>
		/// <returns>True if font loaded successfully, False otherwise.</returns>

		/// <summary>
		/// Releases all the font's resources.
		/// </summary>
		void CloseFont();

		TTF_Font* GetTTF() const { return m_Font; }

	private:
		TTF_Font* m_Font{ nullptr }; // Holds the font information.
	};
}

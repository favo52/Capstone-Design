/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#pragma once

#include <SDL_ttf.h>

namespace Chesster
{
	/* Abstraction of the SDL_ttf addon.
	   Loads fonts from ttf files. Closes fonts when not wanted anymore. */
	class Font
	{
	public:
		/* Loads a ttf file.
		 @param filename The filepath of the ttf file.
		 @param size The size to be set to the font. */
		Font(const std::string& filename, int size = 30);

		/* Calls CloseFont() */
		virtual ~Font();

		/* Releases all the font's resources. */
		void CloseFont();

		/* Holds the font information.
		 @return A pointer to the Font's TTF_Font member variable. */
		TTF_Font* GetTTF() const { return m_Font; }

	private:
		TTF_Font* m_Font; // Holds the font information.
	};
}

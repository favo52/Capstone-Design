/*
* Copyright 2022-present, Francisco A. Villagrasa
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

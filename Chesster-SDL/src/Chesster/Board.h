#pragma once

#include "pch.h"
#include "Window.h"

namespace Chesster
{
	class Board
	{
	public:
		Board(Window* window);
		~Board();

		void Draw();
		void Update();
		void HandleEvent(const SDL_Event& event);

	private:
		Window* m_Window;
	};
}

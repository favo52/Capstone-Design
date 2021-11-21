#pragma once

#include "pch.h"
#include "Window.h"
#include "Connector.h"

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
		Connector m_Connector;

		bool PrintMoves = true; // testing
		std::vector<std::string> m_ValidMoves;
	};
}

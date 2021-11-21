#pragma once

#include "pch.h"
#include "Window.h"
#include "Connector.h"
#include "Texture.h"
#include "ResourceHolder.h"

namespace Chesster
{
	class Board
	{
	public:
		Board(Window* window);
		~Board();

		void Draw();
		bool Update();
		bool HandleEvent(const SDL_Event& event);

	private:
		void LoadPositions();

		std::string ToChessNotation(const Vector2f& position);
		Vector2f ToCoord(char a, char b);

		void Move(const std::string& notation);

		void LoadTextures();
		void PrepareBoard();

	private:
		int* m_Board = new int[8 * 8]
		{
			-1, -2, -3, -4, -5, -3, -2, -1,
			-6, -6, -6, -6, -6, -6, -6, -6,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 6,  6,  6,  6,  6,  6,  6,  6,
			 1,  2,  3,  4,  5,  3,  2,  1
		};

	private:
		Window* m_Window;
		Connector m_Connector;

		TextureHolder m_TextureHolder;
		Texture* m_BoardTexture;
		SDL_Rect m_Bounds;
		SDL_Rect m_PieceClip[32];

		Texture* m_Pieces;
		std::string m_PositionHistory;
		int m_PieceSize;

		bool m_IsMove;
		float m_Dx, m_Dy;
		Vector2f m_OldPos, m_NewPos;
		std::string m_Str;
		int m_PieceIndex;

		Vector2i m_MousePos;
		Vector2f m_BoardOffset;
		float m_PieceOffset;

		bool m_IsComputerTurn;
		unsigned int m_Turn;

		bool m_PrintMoves = false; // testing
		std::vector<std::string> m_ValidMoves;
	};
}

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

		bool HandleEvent(SDL_Event& event);
		bool Update(const std::chrono::duration<double>& dt);
		void Draw();

		void ResetBoard();
		void EvaluateBoard();

	private:
		void LoadPositions();

		std::string ToChessNotation(const Vector2f& position);
		Vector2f ToCoord(char a, char b);

		bool inRange(int low, int high, int x)
		{
			return ((x - high) * (x - low) <= 0);
		}

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

		// Texture stuff
		TextureHolder m_TextureHolder;
		Texture* m_BoardTexture;
		SDL_Rect m_Bounds;
		SDL_Rect m_PieceClip[32];

		Texture* m_Pieces;
		std::string m_PositionHistory;
		int m_MoveHistorySize;
		int m_PieceSize;

		// Piece moving stuff
		bool m_IsMove;
		float m_Dx, m_Dy;
		Vector2f m_OldPos, m_NewPos;
		std::string m_Str;
		int m_PieceIndex;

		// Mouse stuff
		SDL_Point m_MousePos;
		Vector2f m_BoardOffset;
		float m_PieceOffset;
		bool m_HoldingPiece;

		// Computer stuff
		bool m_IsComputerTurn;
		bool m_IsComputerDone;

		bool m_PrintMoves = false; // testing
		std::string m_FEN;
		std::string m_StartPosFEN;
		std::vector<std::string> m_ValidMoves;
		std::string m_PathPythonScript;
	};
}

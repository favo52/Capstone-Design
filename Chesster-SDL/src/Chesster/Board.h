#pragma once

#include "pch.h"
#include "Core/Window.h"
#include "Connector.h"
#include "Core/Texture.h"
#include "Core/ResourceHolder.h"

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

		inline const std::vector<std::string> GetValidMoves() const { return m_ValidMoves; }

	private:
		void LoadPositions();

		std::string ToChessNotation(const Vector2i& position);
		Vector2i ToCoord(char a, char b);

		/*bool inRange(int low, int high, int x)
		{
			return ((x - high) * (x - low) <= 0);
		}*/

		bool IsWhitePawn(const int& index);
		bool IsBlackPawn(const int& index);

		void Move(const std::string& notation);

		void PaintActiveSquares();

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
		Texture* m_Pieces;	// all 32 pieces

		int m_PieceSize;
		SDL_Rect m_Bounds;
		SDL_Rect m_PieceClip[32];
		SDL_FRect m_ActiveSquares[2];

		std::string m_PositionHistory;
		int m_MoveHistorySize;

		// Piece moving stuff
		bool m_IsMove;
		float m_Dx, m_Dy;
		Vector2i m_OldPos, m_NewPos;
		std::string m_Str;
		int m_PieceIndex;

		// Mouse stuff
		SDL_Point m_MousePos;
		Vector2i m_BoardOffset;
		Vector2f m_PieceOffset;
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

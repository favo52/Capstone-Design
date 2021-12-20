#pragma once

#include "Connector.h"
#include "Core/Texture.h"

#include "States/PawnPromotionState.h"

namespace Chesster
{
	class Board : public State
	{
	public:
		enum class Player
		{
			Black = 0, White = 1
		};

		friend Player operator++(Player& menuOption);

		bool m_Gameover;

	public:
		Board(StateStack& stack, Context context);
		virtual ~Board() = default;

		bool HandleEvent(SDL_Event& event);
		bool Update(const std::chrono::duration<double>& dt);
		void Draw();

		void ResetBoard();
		void EvaluateBoard();
		void ChangeDifficulty();

		inline const std::vector<std::string> GetValidMoves() const { return m_ValidMoves; }

		inline const Board::Player GetWinningPlayer() const { return m_CurrentPlayer; }

	private:
		void LoadPositions();

		std::string ToChessNotation(const Vector2i& position);
		Vector2i ToCoord(char a, char b);

		std::vector<std::string> GetAllMissingPieces(const std::vector<std::string>& CameraDataA, const std::vector<std::string>& CameraDataB);
		std::string GetMissingPiece(const std::vector<std::string>& CameraDataA, const std::vector<std::string>& CameraDataB);
		bool IsPresent(const std::string& position, const std::vector<std::string>& CameraDataB);

		bool IsWhitePawn(const int& index);
		bool IsBlackPawn(const int& index);
		bool IsPawn(const int& index);
		bool IsRow(const std::string& notation, char row);

		void CheckPawnPromotion(int offset);
		void ErasePawn(std::vector<int>& pawns);

		void Move(const std::string& notation);
		void ValidateMove();

		void PaintActiveSquares();

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

		std::vector<int> WhitePawns = { 16, 17, 18, 19, 20, 21, 22, 23 };
		std::vector<int> BlackPawns = { 8, 9, 10, 11, 12, 13, 14, 15 };

	private:
		Window* m_Window;
		Connector m_Connector;

		// Texture stuff
		Texture* m_BoardTexture;
		Texture* m_Pieces;	// all 32 pieces

		int m_PieceSize;
		SDL_Rect m_PieceClip[32];
		SDL_FRect m_HighlightedSquares[2];

		// Game history
		std::string m_MoveHistory;
		int m_MoveHistorySize;

		// Piece moving stuff
		bool m_IsDragging;
		float m_Dx, m_Dy;
		Vector2i m_OldPos, m_NewPos;
		std::string m_CurrentMove;
		int m_PieceIndex;
		Vector2i m_OutOfView;

		// Mouse stuff
		SDL_Point m_MousePos;
		Vector2i m_BoardOffset;
		Vector2f m_PieceOffset;
		bool m_HoldingPiece;

		// Computer stuff
		bool m_IsComputerTurn;
		bool m_IsAnimationDone;

		std::string m_FEN;
		std::string m_StartPosFEN;
		std::vector<std::string> m_ValidMoves;
		std::string m_PathPythonScript;

		// Camera stuff
		std::vector<std::string> m_CameraOldData;
		std::vector<std::string> m_CameraNewData;

		bool m_Promoting = false;

		Player m_CurrentPlayer;
	};
}

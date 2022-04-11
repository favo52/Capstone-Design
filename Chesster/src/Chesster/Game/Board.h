#pragma once

#include "Chesster/Renderer/Renderer.h"

/* LEGEND
*/

namespace Chesster
{
	constexpr int SQUARE_SIZE{ 100 };

	class Piece;

	/*	Represents the chess board. */
	class Board
	{
	public:
		/*	Represents an individual square on the board. */
		struct Square
		{
			glm::vec2	Position{ 0.0f, 0.0f };
			glm::vec4	Color{ 0.0f, 0.0f, 0.0f, 1.0f };
			std::string	Notation;

			const glm::vec2 GetCenter() const;
			const RectBounds GetBounds() const;
		};

	public:
		Board(const glm::vec2& viewportSize);

		/*	Draws all the 64 squares of the chess board. */
		void OnRender();

		void OnNewMove(const std::string& currentMove, Piece* currentPiece);

		void OnViewportResize(const glm::vec2& viewportSize);

		void UpdateActiveSquares();
		void ResetActiveSquares();

		/**	Used to retrieve the board squares.
		 @return Returns an array of all 64 individual squares of the board. */
		std::array<Square, 64>& GetBoardSquares() { return m_BoardSquares; }
			
		/** It is used to retrieve the instance of the current Board.
		 @return A reference to this Board object. */
		static Board& Get() { return *s_Instance; }

	private:
		void MovePiece(const std::string& notation);

	private:
		std::array<Square, 64> m_BoardSquares;	// Represents the 64 squares of the chess board
		std::array<Square, 2> m_ActiveSquares;	// The two highlighted squares

		static Board* s_Instance;
	};
}

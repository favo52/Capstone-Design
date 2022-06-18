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

#include "Chesster/Game/Piece.h"

namespace Chesster
{
	constexpr int SQUARE_SIZE{ 100 };	// The size in pixels of all 64 squares.

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
		/* Loads the neccesary resources. */
		Board();

		/* Sets up the properties for all 64 Squares. */
		void ConstructBoard();

		/* Sets up the starting position properties of all 32 Pieces. */
		void ConstructPieces();

		/* Draws all the 64 squares and the 32 Pieces of the chess board. */
		void OnRender();

		/* Updates the screen position of all Squares and Pieces when the windows is resized.
		 @param viewportSize The new width and height of the viewport window. */
		void OnViewportResize(const glm::vec2& viewportSize);

		/* Visually moves a Piece based on the given algebraic notation. */
		void MovePiece(const std::string& notation);		

		/* Searches if a Piece has been captured and updates its status to captured. */
		void UpdatePieceCapture();
		
		/* Checks if the currently made move is a special move, like castling or en passant, and perform the move.
		 @param currentMove The current move in algebraic notation. */
		void UpdateSpecialMove(const std::string& currentMove);

		/* Sets the position of the two active squares to be the current move.
		 @param currentMove The current move in algebraic notation. */
		void UpdateActiveSquares(const std::string& currentMove);
		
		/* Sets the active square to be transparent. */
		void HideActiveSquares();

		/* Used to retrieve the board squares.
		 @return Returns an array of all 64 individual squares of the board. */
		std::array<Square, 64>& GetBoardSquares() { return m_BoardSquares; }

		/* Used to retrieve the array of all chess pieces.
		 @return Returns an array of all 32 individual chess pieces of the board. */
		std::array<Piece, 32>& GetChessPieces() { return m_ChessPieces; }

		/* @return A reference to the currently held piece. */
		Piece& GetCurrentPiece() { return *m_CurrentPiece; }

		/* @return A pointer to the currently clicked piece. */
		Piece* GetClickedPiece() { return m_ClickedPiece; }

		/* @param piece The piece that was clicked with the mouse. */
		void SetClickedPiece(Piece& piece) { m_ClickedPiece = &piece; }

	private:
		std::array<Square, 64> m_BoardSquares;	// Represents the 64 squares of the chess board
		std::array<Square, 2> m_ActiveSquares;	// The two highlighted squares

		std::array<Piece, 32> m_ChessPieces;	// The 32 chess pieces
		
		Piece* m_CurrentPiece;			// A pointer to the currently held piece
		Piece* m_PreviousPiece;			// A pointer to the previously held piece
		Piece* m_ClickedPiece;			// A pointer to the current piece clicked by the mouse

		std::unique_ptr<Texture> m_PieceSpriteSheetTexture;
	};
}

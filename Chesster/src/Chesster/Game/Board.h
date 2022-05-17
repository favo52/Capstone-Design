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

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#pragma once

#include "Chesster/Game/Piece.h"

/* LEGEND
*/

namespace Chesster
{
	constexpr int SQUARE_SIZE{ 100 };

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
		Board();

		/*	Draws all the 64 squares of the chess board. */
		void OnRender();

		void OnViewportResize(const glm::vec2& viewportSize);

		void MovePiece(const std::string& notation);
		void ResetPieces();

		void UpdatePieceCapture();
		void UpdateNewMove(const std::string& currentMove);

		void UpdateActiveSquares(const std::string& currentMove);
		void ResetActiveSquares();

		void SetCurrentPiece(Piece* currentPiece) { m_CurrentPiece = currentPiece; }

		/**	Used to retrieve the board squares.
		 @return Returns an array of all 64 individual squares of the board. */
		std::array<Square, 64>& GetBoardSquares() { return m_BoardSquares; }

		/**	Used to retrieve the board squares.
		 @return Returns an array of all 64 individual squares of the board. */
		std::array<Piece, 32>& GetChessPieces() { return m_ChessPieces; }

		Piece& GetCurrentPiece() { return *m_CurrentPiece; }

	private:
		std::array<Square, 64> m_BoardSquares;	// Represents the 64 squares of the chess board
		std::array<Square, 2> m_ActiveSquares;	// The two highlighted squares

		std::array<Piece, 32> m_ChessPieces;	// The 32 chess pieces
		Piece* m_CurrentPiece;					// A pointer to the currently held piece

		std::unique_ptr<Texture> m_PieceSpriteSheetTexture;
	};
}

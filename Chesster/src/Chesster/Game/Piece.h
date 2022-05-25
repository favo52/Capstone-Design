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

#include "Chesster/Renderer/Renderer.h"

namespace Chesster
{
	constexpr int PIECE_SIZE{ 80 };

	class Piece
	{
	public:
		enum class Type
		{
			None = 0,
			Rook, Knight, Bishop, Queen, King, Pawn
		};

		enum class Color
		{
			None = 0,
			Black, White
		};

	public:
		/*	Allows a Piece object to be created with zero or empty values. */
		Piece();

		/**	Changes a pawn's type to the newly selected one.
		 @param notation The 5 char notation with the selected pawn promotion. */
		void Promote(const std::string& notation);

		/*	Flags the Piece as captured, changes its Notation to "00"
			and sets its Position to a coordinate far away from the screen. */
		void Capture();

		/** Sets the En Passant flag to false if it was set to true on the previous turn. 
			Sets the En Passant flag to true if the Piece has moved two squares during the current turn.
		 @param oldPos The original position of this Piece during the current turn.
		 @param newPos The new position of this Piece during the current turn. */
		void CheckEnPassant(const std::string& oldPos, const std::string& newPos);

		/**	Check wether the Piece is a pawn or not.
		 @return True is the Piece is a pawn, false if not. */
		bool IsPawn();

		bool IsPromotion(const std::string& notation);

		/*	Sets the Piece's TextureClip according to its Type and Color.
			NOTE: The Piece MUST have a Type and Color before this function can be used. */
		void SetTextureClip();

		/**	Changes the position of the chess piece. 
		*	The new (x, y) coordinate must be the Center 
		*	coordinates of a Board Square.
		 @param squareCenter The (x, y) coordinates of the square where the piece is to be moved. */
		void SetPosition(const glm::vec2& squareCenter);

		const std::string& GetNotation() const { return m_Notation; }

		const Color& GetColor() const{ return m_Color; }
		
		/**	Calculates the left, right, bottom, 
			and top coordinates based on its center.
		 @return A RectBounds object containing the coords of all four sides of the Piece. */
		const RectBounds GetBounds() const;

	private:
		SDL_Rect m_TextureClip;		// The texture coordinates on the piece spritesheet
		glm::vec2 m_Position;		// The top left (x, y) positon of the piece
		std::string m_Notation;		// The piece's location in algebraic notation
		Type m_Type;
		Color m_Color;

		bool m_EnPassant;			// True if the piece is a pawn and moved twice
		bool m_IsCaptured;			// True when the piece is captured

		friend class Board;
	};
}

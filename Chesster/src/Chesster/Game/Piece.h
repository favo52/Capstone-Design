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

		void OnViewportResize();

		/**	Changes a pawn's type to the newly selected one.
		 @param notation The 5 char notation with the selected pawn promotion. */
		void Promote(const std::string& notation);

		/*	Flags the Piece as captured, changes its Notation to "00"
			and sets its Position to a coordinate far away from the screen. */
		void Capture();

		/** Sets the En Passant flag to false if it was set to true on the previous turn. 
			Sets the En Passant flag to true if the Piece has moved two squares during the current turn.
		 @param oldPos The original position of this Piece during the current turn. */
		void UpdateEnPassant(const std::string& oldPos);

		/**	Check wether the Piece is a pawn or not.
		 @return True is the Piece is a pawn, false if not. */
		bool IsPawn();

		bool IsPromotion(const std::string& notation);

		/** Check wether the Piece has been captured.
		 @return True if the Piece has been captured, false if not. */
		bool IsCaptured() { return m_IsCaptured; }

		//void SetIndex(uint32_t index) { m_Index = index; }

		/*	Sets the Piece's TextureClip according to its Type and Color.
			NOTE: The Piece MUST have a Type and Color before this function can be used. */
		void SetTextureClip();

		/**	Changes the position of the chess piece. 
		*	The new (x, y) coordinate must be the Center 
		*	coordinates of a Board Square.
		 @param x The new x coordinate.
		 @param y The new y coordinate. */
		void SetPosition(int centerX, int centerY);

		void SetNotation(const std::string& notation) { m_Notation = notation; }

		void SetType(Type type) { m_Type = type; }

		void SetColor(Color color) { m_Color = color; }

		void SetCaptured(bool boolean) { m_IsCaptured = boolean; }

		void SetEnPassant(bool boolean) { m_EnPassant = boolean; }

		//const uint32_t& GetIndex() const { return m_Index; }
		SDL_Rect& GetTextureClip() { return m_TextureClip; }
		const glm::vec2& GetPosition() const { return m_Position; }
		const std::string& GetNotation() const { return m_Notation; }
		const Color& GetColor() const{ return m_Color; }
		
		/**	Calculates the left, right, bottom, 
			and top coordinates based on its center.
		 @return A RectBounds object containing the coords of all four sides of the Piece. */
		const RectBounds GetBounds() const;

	private:
		//uint32_t m_Index;
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

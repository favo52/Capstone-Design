#include "pch.h"
#include "Chesster/Game/Piece.h"

#include "Chesster/Game/Board.h"

namespace Chesster
{
	Piece::Piece() :
		m_Index{ 0 },
		m_TextureClip{ 0, 0, PIECE_SIZE, PIECE_SIZE },
		m_Position{ 0.0f, 0.0f },
		m_Notation{ "00" },
		m_Type{ Type::None },
		m_Color{ Color::None },
		m_EnPassant{ false },
		m_IsCaptured{ false }
	{
	}

	void Piece::OnViewportResize()
	{
		auto& boardSquares = Board::Get().GetBoardSquares();
		auto squareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return sq.Notation == m_Notation; });

		if (squareItr != std::end(boardSquares))
			SetPosition(squareItr->GetCenter().x, squareItr->GetCenter().y);
	}

	void Piece::Promote(const std::string& notation)
	{
		if (IsPawn() && notation.size() > 4)
		{
			switch (notation.back())
			{
				case 'r': m_Type = Type::Rook; break;
				case 'n': m_Type = Type::Knight; break;
				case 'b': m_Type = Type::Bishop; break;
				case 'q': m_Type = Type::Queen; break;
			}

			SetTextureClip();
		}
	}

	void Piece::Capture()
	{
		std::string capturedAtNotation{ m_Notation };

		m_Notation = "00";
		m_Position = { -3000.0f, -3000.0f }; // ridiculous distance offscreen
		m_IsCaptured = true;
	}

	void Piece::CheckEnPassant(const std::string& oldPos)
	{
		if (IsPawn())
		{
			if (m_EnPassant)
				m_EnPassant = false;

			// If pawn moved two squares
			if (((oldPos[1] - '0') + (m_Notation[1] - '0')) % 2 == 0)
				m_EnPassant = true;
		}
	}

	void Piece::SetTextureClip()
	{
		m_TextureClip.x = PIECE_SIZE * ((int)m_Type - 1);
		m_TextureClip.y = PIECE_SIZE * ((int)m_Color - 1);
	}

	void Piece::SetPosition(int centerX, int centerY)
	{
		// Aligns the piece position to be at the center of the square
		m_Position = { centerX - (PIECE_SIZE * 0.5f), centerY - (PIECE_SIZE * 0.5f) };
	}

	void Piece::SetType()
	{
		switch (m_Index)
		{
			case 0: case 7:		// Black
			case 24: case 31:	// White
				m_Type = Type::Rook; break;

			case 1: case 6:
			case 25: case 30:
				m_Type = Type::Knight; break;

			case 2: case 5:
			case 26: case 29:
				m_Type = Type::Bishop; break;

			case 3:
			case 27:
				m_Type = Type::Queen; break;

			case 4:
			case 28:
				m_Type = Type::King; break;

			case 8: case 9: case 10: case 11:
			case 12: case 13: case 14: case 15:
			case 16: case 17: case 18: case 19:
			case 20: case 21: case 22: case 23:
				m_Type = Type::Pawn; break;
		}
	}

	const RectBounds Piece::GetBounds() const
	{
		const glm::vec2 center = { (m_Position.x + (PIECE_SIZE * 0.5f)),
			(m_Position.y + (PIECE_SIZE * 0.5f)) };

		return {
			center.x - (PIECE_SIZE * 0.5f),	// left
			center.x + (PIECE_SIZE * 0.5f),	// right
			center.y - (PIECE_SIZE * 0.5f),	// bottom
			center.y + (PIECE_SIZE * 0.5f)	// top
		};
	}

	bool Piece::IsPawn()
	{
		return (m_Type == Type::Pawn) ?
			true : false;
	}

	bool Piece::IsPromotion(const std::string& notation)
	{
		if (IsPawn())
		{
			if (m_Color == Color::Black && notation[1] == '2' && notation[3] == '1' ||
				m_Color == Color::White && notation[1] == '7' && notation[3] == '8')
			{
				return true;
			}
		}

		return false;
	}
}

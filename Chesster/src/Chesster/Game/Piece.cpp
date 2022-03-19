#include "pch.h"
#include "Piece.h"

#include "Chesster/Game/Board.h"

namespace Chesster
{
	void Piece::OnViewportResize()
	{
		auto x = Board::m_SquaresMap.find(Notation);
		if (x != Board::m_SquaresMap.end())
		{
			auto& square = x->second;
			SetPosition(square->Center.x, square->Center.y);
		}
	}

	void Piece::SetPosition(int x, int y)
	{
		Position = { x - (Size * 0.5f), y - (Size * 0.5f) };
		UpdateCenter();
		UpdateWorldBounds();
	}

	void Piece::SetType()
	{
		switch (Index)
		{
			// Black
			case 0: case 7:
				Type = PieceType::Rook; break;				
			case 1: case 6:
				Type = PieceType::Knight; break;
			case 2: case 5:
				Type = PieceType::Bishop; break;
			case 3:
				Type = PieceType::Queen; break;
			case 4:
				Type = PieceType::King; break;
			case 8: case 9: case 10: case 11:
			case 12: case 13: case 14: case 15:
				Type = PieceType::Pawn; break;

			// White
			case 16: case 17: case 18: case 19:
			case 20: case 21: case 22: case 23:
				Type = PieceType::Pawn; break;
			case 24: case 31:
				Type = PieceType::Rook; break;
			case 25: case 30:
				Type = PieceType::Knight; break;
			case 26: case 29:
				Type = PieceType::Bishop; break;
			case 27:
				Type = PieceType::Queen; break;
			case 28:
				Type = PieceType::King; break;
		}
	}

	void Piece::CheckEnPassant(std::string oldPos)
	{
		if (IsPawn())
		{
			if (EnPassant)
				EnPassant = false;

			if (((oldPos[1] - '0') + (Notation[1] - '0')) % 2 == 0)
				EnPassant = true;
		}
	}

	void Piece::UpdateCenter()
	{
		Center = { (Position.x + (Size * 0.5f)),
			(Position.y + (Size * 0.5f)) };
	}

	void Piece::UpdateWorldBounds()
	{
		WorldBounds.left = Center.x - (Size * 0.5f);
		WorldBounds.right = Center.x + (Size * 0.5f);
		WorldBounds.bottom = Center.y - (Size * 0.5f);
		WorldBounds.top = Center.y + (Size * 0.5f);
	}

	void Piece::UpdateTextureClip(std::string notation)
	{
		int color = (Color == PieceColor::White) ? 1 : 0;
		constexpr int pieceSize{ 80 };

		char upgrade{ notation.back() };
		switch (upgrade)
		{
			case 'r':
			{
				m_TextureClip = { pieceSize * 0, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Rook;
				break;
			}

			case 'n':
			{
				m_TextureClip = { pieceSize * 1, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Knight;
				break;
			}

			case 'b':
			{
				m_TextureClip = { pieceSize * 2, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Bishop;
				break;
			}

			case 'q':
			{
				m_TextureClip = { pieceSize * 3, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Queen;
				break;
			}

			default:
				return;

		}
	}

	bool Piece::IsPawn()
	{
		if (Type == PieceType::Pawn)
			return true;

		return false;
	}

	bool Piece::IsPromotion(std::string notation)
	{
		if (IsPawn())
		{
			if (Color == PieceColor::Black && notation[1] == '2' && notation[3] == '1' ||
				Color == PieceColor::White && notation[1] == '7' && notation[3] == '8')
			{
				return true;
			}
		}

		return false;
	}
}

#include "pch.h"
#include "Piece.h"

#include "Chesster_Unleashed/Game/Board.h"

namespace Chesster
{
	void Piece::OnUpdate(const std::chrono::duration<double>& dt)
	{
	}

	void Piece::OnRender() const
	{
		Renderer::DrawTextureEx(&Texture);
	}

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
		Position = { x - (Size.x * 0.5f), y - (Size.y * 0.5f), 0.0f };
		Texture.SetPosition(Position.x, Position.y);
		UpdateCenter();
		UpdateWorldBounds();
	}

	void Piece::SetPieceClips(std::array<SDL_Rect, 32>& pieceClips)
	{
		int* pieceLocations = new int[8 * 8]
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

		constexpr int RANK{ 8 };
		constexpr int SIZE{ 80 };

		int pieceIndex{ 0 };
		for (int y = 0; y < RANK; y++)
		{
			for (int x = 0; x < RANK; x++)
			{
				int boardSquare = pieceLocations[x + y * RANK];
				if (!boardSquare)
					continue;

				int xPos = abs(boardSquare) - 1;
				int yPos = (boardSquare > 0) ? 1 : 0;

				// Prepare piece clipping and position
				pieceClips[pieceIndex] = { SIZE * xPos, SIZE * yPos, SIZE, SIZE };
				++pieceIndex;
			}
		}

		delete[] pieceLocations;
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
		Center = { (Position.x + (Size.x * 0.5f)),
			(Position.y + (Size.y * 0.5f)) };
	}

	void Piece::UpdateWorldBounds()
	{
		WorldBounds.left = Center.x - (Size.x * 0.5f);
		WorldBounds.right = Center.x + (Size.x * 0.5f);
		WorldBounds.bottom = Center.y - (Size.y * 0.5f);
		WorldBounds.top = Center.y + (Size.y * 0.5f);
	}

	void Piece::UpdateTextureClip(std::string notation, std::array<SDL_Rect, 32>& pieceClips)
	{
		int color = (Color == PieceColor::White) ? 1 : 0;
		constexpr int pieceSize{ 80 };

		char upgrade{ notation.back() };
		LOG_INFO("notation.back()", upgrade);
		SDL_Rect clip{};
		switch (upgrade)
		{
			case 'r':
			{
				pieceClips[Index] = { pieceSize * 0, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Rook;
				break;
			}

			case 'n':
			{
				pieceClips[Index] = { pieceSize * 1, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Knight;
				break;
			}

			case 'b':
			{
				pieceClips[Index] = { pieceSize * 2, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Bishop;
				break;
			}

			case 'q':
			{
				pieceClips[Index] = { pieceSize * 3, pieceSize * color, pieceSize, pieceSize };
				Type = PieceType::Queen;
				break;
			}

			default:
				return;

		}

		Texture.SetClip(&pieceClips[Index]);
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

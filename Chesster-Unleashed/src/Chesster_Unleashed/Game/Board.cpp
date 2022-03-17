#include "pch.h"
#include "Chesster_Unleashed/Game/Board.h"

#include "Chesster_Unleashed/Renderer/Framebuffer.h"

namespace Chesster
{
	std::array<Board::Square, 64> Board::m_BoardSquares{};
	std::unordered_map<std::string, Board::Square*> Board::m_SquaresMap;

	void Board::Init(const glm::vec2& viewportSize)
	{
		glm::vec4 BlackColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 BlueColor = { 0.084f, 0.342f, 0.517f, 1.0f };
		glm::vec4 WhiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		glm::vec2 offset{ (viewportSize.x * 0.5) - 600.0f , (viewportSize.y * 0.5f) - 425.0f };

		// Iterate all 64 squares
		for (size_t x = 0; x < 8; ++x)
		{
			char file = 'a' + x;
			for (size_t y = 0; y < 8; ++y)
			{
				char rank = '8' - y;
				std::string squareNotation{ file };
				squareNotation += rank;

				// Set each square's properties
				Square* square = &m_BoardSquares[x + y * 8];
				glm::vec4 CurrentColor = (x + y) % 2 == 0 ? WhiteColor : BlueColor;
				square->Color = CurrentColor * 255.0f;
				square->Position = { (float)x * square->Size.x + offset.x, (float)y * square->Size.y + offset.y };
				square->Notation = squareNotation;
				square->Index = x + y;

				square->UpdateCenter();
				square->UpdateWorldBounds();

				m_SquaresMap[squareNotation] = square;
			}
		}
	}

	void Board::OnUpdate(const std::chrono::duration<double>& dt)
	{
		PaintActiveSquares();
	}

	void Board::OnRender()
	{
		// Draw board
		for (const Square& square : m_BoardSquares)
		{
			SDL_Rect rect = { square.Position.x, square.Position.y, square.Size.x, square.Size.y };
			Renderer::DrawFilledRect(rect, square.Color);
		}

		// Draw active squares
		SDL_Rect oldSq = { m_ActiveSquares[0].Position.x, m_ActiveSquares[0].Position.y, 100, 100 };
		Renderer::DrawFilledRect(oldSq, m_ActiveSquares[0].Color);

		SDL_Rect newSq = { m_ActiveSquares[1].Position.x, m_ActiveSquares[1].Position.y, 100, 100 };
		Renderer::DrawFilledRect(newSq, m_ActiveSquares[1].Color);
	}

	void Board::OnNewMove(std::array<Piece, 32>& pieces, const std::string& currentMove, int pieceIndex)
	{
		m_Pieces = &pieces;
		m_CurrentMove = currentMove;

		std::string oldPos{ currentMove[0], currentMove[1] };
		std::string newPos{ currentMove[2], currentMove[3] };

		// Castling					// Move Rook
		if (currentMove == "e1g1") Castle("h1f1");
		if (currentMove == "e8g8") Castle("h8f8");
		if (currentMove == "e1c1") Castle("a1d1");
		if (currentMove == "e8c8") Castle("a8d8");

		Piece* currentPiece{ &pieces[pieceIndex] };
		if (currentPiece->IsPawn())
		{
			/////////////////////////////////////////////////////////////////
			//// En Passant /////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////
			PieceColor currentPawnColor{ pieces[pieceIndex].Color };

			int offset{ 100 };
			if (currentPawnColor == PieceColor::Black)
				offset *= -1;

			// Grab the piece behind
			Piece* pieceBehind{ nullptr };
			for (auto& piece : pieces)
			{
				if (piece.Notation[0] == currentPiece->Notation[0] &&
					piece.Center.y == currentPiece->Center.y + offset)
				{
					pieceBehind = &piece;
				}
			}

			// Capture the correct pawn
			if (pieceBehind)
			{
				LOG_INFO(pieceBehind->EnPassant);
				if (pieceBehind->IsPawn() && pieceBehind->Color != currentPiece->Color &&
					pieceBehind->EnPassant)
				{
					pieceBehind->Notation = "00";
					pieceBehind->SetPosition(-3000.0f, -3000.0f);
				}
			}
		}
	}

	void Board::OnViewportResize(const glm::vec2& viewportSize)
	{
		// Offset is half of viewport size minus half of entire board's size
		glm::vec2 offset{ (viewportSize.x * 0.5) - (m_BoardSquares[0].Size.x * 8.0f) * 0.5f ,
						(viewportSize.y * 0.5f) - (m_BoardSquares[0].Size.x * 8.0f) * 0.5f};

		for (size_t x = 0; x < 8; ++x)
		{
			for (size_t y = 0; y < 8; ++y)
			{
				Square* square = &m_BoardSquares[x + y * 8];
				square->Position = { (float)x * square->Size.x + offset.x, (float)y * square->Size.y + offset.y };
				square->UpdateCenter();
				square->UpdateWorldBounds();
			}
		}
	}

	void Board::Reset()
	{
		m_CurrentMove = "0000";
		m_ActiveSquares[0].Color = { 0, 0, 0, 0 };
		m_ActiveSquares[1].Color = { 0, 0, 0, 0 };
	}

	void Board::Castle(const std::string& notation)
	{
		std::string oldPos{ notation[0], notation[1] };
		std::string newPos{ notation[2], notation[3] };

		auto newSquare = m_SquaresMap.find(newPos);
		if (newSquare != m_SquaresMap.end())
		{
			auto& square = newSquare->second;

			// Move rook
			for (Piece &piece : *m_Pieces)
			{
				if (piece.Notation == oldPos)
				{
					piece.SetPosition(square->Center.x, square->Center.y);
					piece.Notation = newPos;
					piece.UpdateCenter();
					piece.UpdateWorldBounds();
				}
			}
		}		
	}

	void Board::PaintActiveSquares()
	{
		std::string oldPos = { m_CurrentMove[0], m_CurrentMove[1] };
		std::string newPos = { m_CurrentMove[2], m_CurrentMove[3] };
		
		auto oldSquare = m_SquaresMap.find(oldPos);
		auto newSquare = m_SquaresMap.find(newPos);
		if (oldSquare != m_SquaresMap.end() && newSquare != m_SquaresMap.end())
		{
			// Position
			m_ActiveSquares[0] = *oldSquare->second;
			m_ActiveSquares[1] = *newSquare->second;

			// Color
			glm::vec4 highlightColor = { 255.0f, 215.0f, 0.0f, 80.0f };
			m_ActiveSquares[0].Color = highlightColor;
			m_ActiveSquares[1].Color = highlightColor;
		}
	}

	void Board::Square::UpdateCenter()
	{
		Center = { (Position.x + (Size.x * 0.5f)),
			(Position.y + (Size.y * 0.5f)) };
	}

	void Board::Square::UpdateWorldBounds()
	{
		WorldBounds.left = Center.x - (Size.x * 0.5f);
		WorldBounds.right = Center.x + (Size.x * 0.5f);
		WorldBounds.bottom = Center.y - (Size.y * 0.5f);
		WorldBounds.top = Center.y + (Size.y * 0.5f);
	}
}

#include "pch.h"
#include "Chesster/Game/Board.h"

#include "Chesster/Layers/GameLayer.h"

#include "Chesster/Game/Piece.h"

namespace Chesster
{
	Board* Board::s_Instance{ nullptr };

	Board::Board(const glm::vec2& viewportSize)
	{
		s_Instance = this;

		const glm::vec4 BlueColor = { 0.084f, 0.342f, 0.517f, 1.0f };
		const glm::vec4 WhiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		const glm::vec2 offset{ (viewportSize.x * 0.5) - (SQUARE_SIZE * 8.0f) * 0.5f ,
						(viewportSize.y * 0.5f) - (SQUARE_SIZE * 8.0f) * 0.5f };

		// Create all 64 squares
		for (size_t x = 0; x < 8; ++x)
		{
			const char file = 'a' + x;
			for (size_t y = 0; y < 8; ++y)
			{
				// Set corresponding notation
				const char rank = '8' - y;
				std::string squareNotation{ file };
				squareNotation += rank;

				// Set corresponding color
				const glm::vec4 color = ((x + y) % 2 == 0) ? WhiteColor : BlueColor;

				// Set each square's properties
				Square* square = &m_BoardSquares[x + y * 8];
				square->Position = { (float)x * SQUARE_SIZE + offset.x, (float)y * SQUARE_SIZE + offset.y };
				square->Color = color * 255.0f;
				square->Notation = squareNotation;
			}
		}
	}

	void Board::OnRender()
	{
		// Draw board
		for (const Square& square : m_BoardSquares)
		{
			const SDL_Rect rect = { square.Position.x, square.Position.y, SQUARE_SIZE, SQUARE_SIZE };
			Renderer::DrawFilledRect(rect, square.Color);
		}

		// Draw active squares
		const SDL_Rect oldSq = { m_ActiveSquares[0].Position.x, m_ActiveSquares[0].Position.y, SQUARE_SIZE , SQUARE_SIZE };
		Renderer::DrawFilledRect(oldSq, m_ActiveSquares[0].Color);

		const SDL_Rect newSq = { m_ActiveSquares[1].Position.x, m_ActiveSquares[1].Position.y, SQUARE_SIZE, SQUARE_SIZE };
		Renderer::DrawFilledRect(newSq, m_ActiveSquares[1].Color);
	}

	void Board::OnViewportResize(const glm::vec2& viewportSize)
	{
		// Offset is half of viewport size minus half of entire board's size
		const glm::vec2 offset{ (viewportSize.x * 0.5) - (SQUARE_SIZE * 8.0f) * 0.5f ,
						(viewportSize.y * 0.5f) - (SQUARE_SIZE * 8.0f) * 0.5f };

		for (size_t x = 0; x < 8; ++x)
		{
			for (size_t y = 0; y < 8; ++y)
			{
				m_BoardSquares[x + y * 8].Position =
				{ (float)x * SQUARE_SIZE + offset.x, (float)y * SQUARE_SIZE + offset.y };
			}
		}
	}

	void Board::ResetActiveSquares()
	{
		m_ActiveSquares[0].Color = { 0, 0, 0, 0 };
		m_ActiveSquares[1].Color = { 0, 0, 0, 0 };
	}

	void Board::UpdateActiveSquares()
	{
		const std::string currentMove = GameLayer::Get().GetCurrentMove();
		if (currentMove.empty()) return;

		const std::string oldPos{ currentMove[0], currentMove[1] };
		const std::string newPos{ currentMove[2], currentMove[3] };
		
		auto oldSquareItr = std::find_if(std::begin(m_BoardSquares), std::end(m_BoardSquares),
			[&](const Board::Square& sq) { return sq.Notation == oldPos; });

		auto newSquareItr = std::find_if(std::begin(m_BoardSquares), std::end(m_BoardSquares),
			[&](const Board::Square& sq) { return sq.Notation == newPos; });

		if (oldSquareItr != std::end(m_BoardSquares) && newSquareItr != std::end(m_BoardSquares))
		{
			// Position
			m_ActiveSquares[0] = *oldSquareItr;
			m_ActiveSquares[1] = *newSquareItr;

			// Color
			const glm::vec4 highlightColor = { 255.0f, 215.0f, 0.0f, 80.0f };
			m_ActiveSquares[0].Color = highlightColor;
			m_ActiveSquares[1].Color = highlightColor;
		}
	}

	const glm::vec2 Board::Square::GetCenter() const
	{
		return { (Position.x + (SQUARE_SIZE * 0.5f)),
			(Position.y + (SQUARE_SIZE * 0.5f)) };
	}

	const RectBounds Board::Square::GetBounds() const
	{
		const glm::vec2 center = GetCenter();

		return {
			center.x - (SQUARE_SIZE * 0.5f), // left
			center.x + (SQUARE_SIZE * 0.5f), // right
			center.y - (SQUARE_SIZE * 0.5f), // bottom
			center.y + (SQUARE_SIZE * 0.5f)	 // top
		};
	}
}

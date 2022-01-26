#include "pch.h"
#include "Chesster_Unleashed/Game/Board.h"

#include "Chesster_Unleashed/Renderer/Framebuffer.h"

namespace Chesster
{
	Board::Board()
	{
	}

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
				BoardSquare* square = &m_BoardSquares[x + y * 8];
				glm::vec4 CurrentColor = (x + y) % 2 == 0 ? BlueColor : WhiteColor;
				square->Color = CurrentColor * 255.0f;
				square->Position = { (float)x * square->Size.x + offset.x, (float)y * square->Size.y + offset.y };
				square->Notation = squareNotation;
				square->Index = x + y;

				square->UpdateCenter();
				square->UpdateWorldBounds();

				m_SquaresMap[squareNotation] = *square;
			}
		}
	}

	void Board::OnUpdate(const std::chrono::duration<double>& dt)
	{
	}

	void Board::OnRender()
	{
		for (const BoardSquare& square : m_BoardSquares)
		{
			SDL_Rect rect = { square.Position.x, square.Position.y, square.Size.x, square.Size.y };
			Renderer::DrawFilledRect(rect, square.Color);
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
				BoardSquare* square = &m_BoardSquares[x + y * 8];
				square->Position = { (float)x * square->Size.x + offset.x, (float)y * square->Size.y + offset.y };
				square->UpdateCenter();
				square->UpdateWorldBounds();
			}
		}
	}

	void Board::BoardSquare::UpdateCenter()
	{
		Center = { (Position.x + (Size.x * 0.5f)),
			(Position.y + (Size.y * 0.5f)) };
	}

	void Board::BoardSquare::UpdateWorldBounds()
	{
		WorldBounds.left = Center.x - (Size.x * 0.5f);
		WorldBounds.right = Center.x + (Size.x * 0.5f);
		WorldBounds.bottom = Center.y - (Size.y * 0.5f);
		WorldBounds.top = Center.y + (Size.y * 0.5f);
	}
}

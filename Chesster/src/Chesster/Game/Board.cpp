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

#include "pch.h"
#include "Chesster/Game/Board.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	Board::Board() :
		m_CurrentPiece{ nullptr }
	{
		// Load and setup the sprite sheet image
		m_PieceSpriteSheetTexture = std::make_unique<Texture>("assets/textures/ChessPieces.png");
		m_PieceSpriteSheetTexture->SetWidth(PIECE_SIZE);
		m_PieceSpriteSheetTexture->SetHeight(PIECE_SIZE);

		const glm::vec4 BlueColor = { 0.084f, 0.342f, 0.517f, 1.0f };
		const glm::vec4 WhiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };

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
				square->Color = color * 255.0f;
				square->Notation = squareNotation;
			}
		}

		ResetPieces();
	}

	void Board::OnRender()
	{
		// Draw board
		for (const Square& square : m_BoardSquares)
		{
			const SDL_Rect& rect = { (int)square.Position.x, (int)square.Position.y, SQUARE_SIZE, SQUARE_SIZE };
			Renderer::DrawFilledRect(rect, square.Color);
		}

		// Draw active squares
		const SDL_Rect& oldSq = { (int)m_ActiveSquares[0].Position.x, (int)m_ActiveSquares[0].Position.y, SQUARE_SIZE , SQUARE_SIZE };
		Renderer::DrawFilledRect(oldSq, m_ActiveSquares[0].Color);

		const SDL_Rect& newSq = { (int)m_ActiveSquares[1].Position.x, (int)m_ActiveSquares[1].Position.y, SQUARE_SIZE, SQUARE_SIZE };
		Renderer::DrawFilledRect(newSq, m_ActiveSquares[1].Color);

		// Draw all the chess pieces
		for (Piece& piece : m_ChessPieces)
		{
			if (piece.m_IsCaptured) continue;	// don't draw captured pieces
			m_PieceSpriteSheetTexture->SetClip(&piece.m_TextureClip);
			m_PieceSpriteSheetTexture->SetPosition(piece.m_Position.x, piece.m_Position.y);
			Renderer::DrawTexture(m_PieceSpriteSheetTexture);
		}

		// Draw the selected chess piece on top of all other chess pieces
		m_PieceSpriteSheetTexture->SetClip(&m_CurrentPiece->m_TextureClip);
		m_PieceSpriteSheetTexture->SetPosition(m_CurrentPiece->m_Position.x, m_CurrentPiece->m_Position.y);
		Renderer::DrawTexture(m_PieceSpriteSheetTexture);
	}

	void Board::OnViewportResize(const glm::vec2& viewportSize)
	{
		// Offset is half of viewport size minus half of entire board's size
		const glm::vec2 offset{ (viewportSize.x * 0.5f) - (SQUARE_SIZE * 8.0f) * 0.5f ,
						(viewportSize.y * 0.5f) - (SQUARE_SIZE * 8.0f) * 0.5f };

		// Update board squares position
		for (size_t x = 0; x < 8; ++x)
		{
			for (size_t y = 0; y < 8; ++y)
			{
				m_BoardSquares[x + y * 8].Position =
				{ (float)x * SQUARE_SIZE + offset.x, (float)y * SQUARE_SIZE + offset.y };
			}
		}

		// Update piece positions
		for (auto& piece : m_ChessPieces)
		{
			auto squareItr = std::find_if(std::begin(m_BoardSquares), std::end(m_BoardSquares),
				[&](const Board::Square& sq) { return sq.Notation == piece.m_Notation; });

			if (squareItr != std::end(m_BoardSquares))
				piece.SetPosition(squareItr->GetCenter());
		}
	}

	void Board::MovePiece(const std::string& notation)
	{
		if (notation.empty() || notation.size() < 4) return;

		// Grab the selected chess piece's old and new positions
		const std::string& oldPos{ notation[0], notation[1] };
		const std::string& newPos{ notation[2], notation[3] };

		// Find new square
		auto targetSquareItr = std::find_if(std::begin(m_BoardSquares), std::end(m_BoardSquares),
			[&](const Board::Square& sq) { return sq.Notation == newPos; });

		if (targetSquareItr != std::end(m_BoardSquares))
		{
			// Find piece and move it to new square
			for (Piece& piece : m_ChessPieces)
			{
				piece.UpdateEnPassant(oldPos);
				if (piece.m_Notation == oldPos)
				{
					m_CurrentPiece = &piece;

					piece.SetPosition(targetSquareItr->GetCenter());
					piece.m_Notation = newPos;
					break;
				}
			}
		}
	}

	void Board::ResetPieces()
	{
		// The numbers are according to the enum class Piece::Type values
		int pieceLocations[8 * 4] =
		{
			1, 2, 3, 4, 5, 3, 2, 1,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			1, 2, 3, 4, 5, 3, 2, 1
		};

		size_t index{ 0 };
		uint32_t offset{ 0 };
		Piece::Color color{ Piece::Color::Black };

		for (Piece& piece : m_ChessPieces)
		{
			if (index > 15) { offset = 32; color = Piece::Color::White; }

			// Set up the piece properties
			piece.SetPosition(m_BoardSquares[index + offset].GetCenter());
			piece.m_Notation = m_BoardSquares[index + offset].Notation;
			piece.m_Type = Piece::Type(pieceLocations[index]);
			piece.m_Color = color;
			piece.SetTextureClip();
			piece.m_EnPassant = false;
			piece.m_IsCaptured = false;
			++index;
		}

		m_CurrentPiece = &m_ChessPieces[0];
	}

	void Board::UpdatePieceCapture()
	{
		for (Piece& piece : m_ChessPieces)
		{
			if (m_CurrentPiece->m_Notation == piece.m_Notation &&
				!(m_CurrentPiece->m_Color == piece.m_Color))	// don't capture self
			{
				piece.Capture();
				GameLayer::Get().UpdateRobotCode(Code::Capture, '1');
				break;
			}
		}
	}

	void Board::UpdateNewMove(const std::string& currentMove)
	{
		if (currentMove.empty()) return;

		// Check for Castling
		bool castling{ false };
		std::string specialMove;						// Move Rook
		if (currentMove == "e1g1") { specialMove = "h1"; MovePiece("h1f1"); castling = true; }
		if (currentMove == "e1c1") { specialMove = "a1"; MovePiece("a1d1"); castling = true; }
		if (currentMove == "e8g8") { specialMove = "h8"; MovePiece("h8f8"); castling = true; }
		if (currentMove == "e8c8") { specialMove = "a8"; MovePiece("a8d8"); castling = true; }
		if (castling)
		{
			GameLayer& gameLayer = GameLayer::Get();

			gameLayer.UpdateRobotCode(Code::Special, '2');
			gameLayer.UpdateRobotCode(Code::SpecialCol, specialMove[0]);
			gameLayer.UpdateRobotCode(Code::SpecialRow, specialMove[1]);
		}

		// Check for En Passant
		if (m_CurrentPiece->IsPawn())
		{
			int offset{ 1 };
			if (m_CurrentPiece->m_Color == Piece::Color::White)
				offset = -1;

			// Grab the piece behind
			Piece* pieceBehind{ nullptr };
			for (Piece& piece : m_ChessPieces)
			{
				if (piece.m_Notation[0] == m_CurrentPiece->m_Notation[0] &&
					piece.m_Notation[1] == m_CurrentPiece->m_Notation[1] + offset)
				{
					pieceBehind = &piece;
				}
			}

			// Capture the correct pawn when en passant
			if (pieceBehind)
			{
				if (pieceBehind->IsPawn() && pieceBehind->m_Color != m_CurrentPiece->m_Color &&
					pieceBehind->m_EnPassant)
				{
					GameLayer& gameLayer = GameLayer::Get();
					gameLayer.UpdateRobotCode(Code::Special, '1');
					gameLayer.UpdateRobotCode(Code::SpecialCol, pieceBehind->m_Notation[0]);
					gameLayer.UpdateRobotCode(Code::SpecialRow, pieceBehind->m_Notation[1]);
					pieceBehind->Capture();
				}
			}
		}
	}

	void Board::ResetActiveSquares()
	{
		m_ActiveSquares[0].Color = { 0, 0, 0, 0 };
		m_ActiveSquares[1].Color = { 0, 0, 0, 0 };
	}

	void Board::UpdateActiveSquares(const std::string& currentMove)
	{
		if (currentMove.empty() || currentMove.size() < 4) return;

		const std::string& oldPos{ currentMove[0], currentMove[1] };
		const std::string& newPos{ currentMove[2], currentMove[3] };

		auto findPos = [&](const std::string& pos)
		{
			return std::find_if(std::begin(m_BoardSquares), std::end(m_BoardSquares),
				[&](const Board::Square& sq) { return sq.Notation == pos; });
		};
		
		auto oldSquareItr = findPos(oldPos);
		auto newSquareItr = findPos(newPos);

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

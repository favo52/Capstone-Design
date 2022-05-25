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
#include "Chesster/Game/Piece.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	Piece::Piece() :
		m_TextureClip{ 0, 0, PIECE_SIZE, PIECE_SIZE },
		m_Position{ -3000.0f, -3000.0f },
		m_Notation{ "00" },
		m_Type{ Type::None },
		m_Color{ Color::None },
		m_EnPassant{ false },
		m_IsCaptured{ false }
	{
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
		m_Notation = "00";
		m_Position = { -3000.0f, -3000.0f }; // ridiculous distance offscreen
		m_IsCaptured = true;
	}

	void Piece::CheckEnPassant(const std::string& oldPos, const std::string& newPos)
	{
		if (IsPawn())
		{
			// Check if a Pawn moved two squares
			if ((((oldPos[1] - '0') + (newPos[1] - '0')) % 2) == 0)
				m_EnPassant = true;
		}
	}

	void Piece::SetTextureClip()
	{
		if (m_Type == Type::None || m_Color == Color::None)
		{
			LOG_WARN("Unable to set Texture Clip. Piece is missing Type or Color.");
			return;
		}

		m_TextureClip.x = PIECE_SIZE * ((int)m_Type - 1);
		m_TextureClip.y = PIECE_SIZE * ((int)m_Color - 1);
	}

	void Piece::SetPosition(const glm::vec2& squareCenter)
	{
		// Aligns the piece position to be at the center of the square
		m_Position = { squareCenter.x - (PIECE_SIZE * 0.5f), squareCenter.y - (PIECE_SIZE * 0.5f) };
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

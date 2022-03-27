#pragma once

#include "Chesster/Renderer/Renderer.h"

namespace Chesster
{
	/// <summary>
	/// 
	/// </summary>
	enum class PieceType
	{
		None = 0,
		King, Queen, Rook, Bishop, Knight, Pawn
	};

	/// <summary>
	/// 
	/// </summary>
	enum class PieceColor
	{
		None = 0,
		White, Black
	};

	class Piece
	{
	public:
		Piece() = default;
		virtual ~Piece() = default;

		void OnViewportResize();

		void SetPosition(int x, int y);
		void SetType();

		void CheckEnPassant(std::string oldPos);

		void UpdateCenter();
		void UpdateWorldBounds();
		void UpdateTextureClip(std::string notation);

		bool IsPawn();
		bool IsPromotion(std::string notation);

	public:
		SDL_Rect m_TextureClip{ 0, 0, 0, 0 };
		glm::vec2 Position = { 0.0f, 0.0f };
		glm::vec2 Center{ 0.0f, 0.0f };
		const float Size{ 80.0f };
		RectBounds WorldBounds{};
		std::string Notation{};
		uint32_t Index{ 0 };
		PieceType Type{ PieceType::None };
		PieceColor Color{ PieceColor::None };
		bool EnPassant{ false };
		bool IsCaptured{ false };
	};
}

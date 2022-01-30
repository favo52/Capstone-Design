#pragma once

#include "Chesster_Unleashed/Renderer/Renderer.h"
#include "Chesster_Unleashed/Renderer/Texture.h"

#include <glm/glm.hpp>


namespace Chesster
{
	struct Square;

	enum class PieceType
	{
		None = 0,
		King, Queen, Rook, Bishop, Knight, Pawn
	};

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

		void OnUpdate(const std::chrono::duration<double>&dt);
		void OnRender() const; 

		void OnViewportResize();

		void SetPosition(int x, int y);
		void SetType();
		void CheckEnPassant(std::string oldPos);

		void UpdateCenter();
		void UpdateWorldBounds();

		bool IsPawn();

	public:
		static void SetPieceClips(std::array<SDL_Rect, 32>& pieceClips);

	public:
		Texture Texture;
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 Size{ 80.0f, 80.0f };
		glm::vec2 Center{ 0.0f, 0.0f };
		QuadBounds WorldBounds{};
		std::string Notation{};
		uint32_t Index{ 0 };
		PieceType Type{ PieceType::None };
		PieceColor Color{ PieceColor::None };
		bool EnPassant{ false };
	};
}

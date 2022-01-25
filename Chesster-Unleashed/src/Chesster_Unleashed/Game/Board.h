#pragma once

#include "Chesster_Unleashed/Renderer/Renderer.h"

#include <glm/glm.hpp>

namespace Chesster
{
	class Board
	{
	public:
		struct BoardSquare
		{
			glm::vec2 Position = { 0.0f, 0.0f };
			glm::vec2 Size{ 100.0f, 100.0f };
			glm::vec4 Color = { 0.0f, 0.0f, 0.0f, 1.0f };
			glm::vec2 Center{ 0.0f, 0.0f };
			QuadBounds WorldBounds{};
			std::string Notation{};
			uint32_t Index{ 0 };
			bool UnderAttack{ false };

			void UpdateCenter();
			void UpdateWorldBounds();
		};

	public:
		Board();
		virtual ~Board() = default;

		void Init(const glm::vec2& viewportSize);

		void OnUpdate(const std::chrono::duration<double>& dt);
		void OnRender();

		void OnViewportResize(const glm::vec2& viewportSize);

		std::array<BoardSquare, 64>& GetBoardSquares() { return m_BoardSquares; }
		const std::array<BoardSquare, 64>& GetBoardSquares() const { return m_BoardSquares; }

		std::unordered_map<std::string, BoardSquare>& GetSquaresMap() { return m_SquaresMap; }
		const std::unordered_map<std::string, BoardSquare>& GetSquaresMap() const { return m_SquaresMap; }

	private:
		std::array<BoardSquare, 64> m_BoardSquares;
		std::unordered_map<std::string, BoardSquare> m_SquaresMap;
	};
}

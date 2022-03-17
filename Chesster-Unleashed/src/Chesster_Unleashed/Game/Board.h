#pragma once

#include "Chesster_Unleashed/Game/Piece.h"

/* LEGEND
<summary> Beginning of the summary.
*/

namespace Chesster
{
	class Board
	{
	public:
		/// <summary>
		/// Represents an individual square in the board.
		/// </summary>
		struct Square
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
		Board() = default;
		virtual ~Board() = default;

		void Init(const glm::vec2& viewportSize);

		void OnUpdate(const std::chrono::duration<double>& dt);

		/// <summary>
		/// Draws all the 64 squares of the chess board.
		/// </summary>
		void OnRender();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pieces"></param>
		/// <param name="currentMove"></param>
		/// <param name="pieceIndex"></param>
		void OnNewMove(std::array<Piece, 32>& pieces, const std::string& currentMove, int pieceIndex);
		void OnViewportResize(const glm::vec2& viewportSize);

		void Reset();

		/// <summary>
		/// Used to retrieve the board.
		/// </summary>
		/// <returns>Returns the entire board.</returns>
		static std::array<Square, 64>& GetBoardSquares() { return m_BoardSquares; }

		std::unordered_map<std::string, Square*>& GetSquaresMap() { return m_SquaresMap; }
		const std::unordered_map<std::string, Square*>& GetSquaresMap() const { return m_SquaresMap; }

	private:
		void Castle(const std::string& notation);
		void PaintActiveSquares();

	public:
		static std::unordered_map<std::string, Square*> m_SquaresMap;

	private:
		static std::array<Square, 64> m_BoardSquares;	// Represents the 64 squares of the chess board
		std::array<Piece, 32>* m_Pieces{ nullptr };		// Represents the 32 pieces of chess

		std::array<Square, 2> m_ActiveSquares{};		// The highlighted squares

		std::string m_CurrentMove{ "0000" };
	};
}

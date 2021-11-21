#include "pch.h"
#include "Board.h"

namespace Chesster
{
	Board::Board(Window* window) :
		m_Window{ window },
		m_ValidMoves{}
	{
		
	}

	Board::~Board()
	{
	}

	void Board::Draw()
	{

	}

	void Board::Update()
	{
		/*
		* This is where we would be parsing stockfish's FEN after updating the board
		* to get only valid moves from Chessnut
		*/

		std::string fen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
		
		if (PrintMoves) // testing, need to do it once
		{
			m_ValidMoves = m_Connector.GetValidMoves(fen);
			
			// print all moves
			for (const std::string& move : m_ValidMoves)
				std::cout << move << " \n";

			PrintMoves = false;
		}
	}

	void Board::HandleEvent(const SDL_Event& event)
	{

	}
}

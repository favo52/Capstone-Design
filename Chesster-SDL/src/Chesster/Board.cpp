#include "pch.h"
#include "Board.h"

namespace Chesster
{
	constexpr int TOTAL_PIECES{ 32 };

	Board::Board(Window* window) :
		m_Window{ window },
		m_BoardTexture{ nullptr },
		m_Bounds{},
		m_Pieces{ new Texture[TOTAL_PIECES] },
		m_PieceSize{ 80 },
		m_IsMove{ false },
		m_Dx{ 0.0f },
		m_Dy{ 0.0f },
		m_OldPos{},
		m_NewPos{},
		m_Str{ "" },
		m_PieceIndex{ 0 },
		m_PositionHistory{ "" },
		m_BoardOffset{ 23.0f, 23.0f },
		m_PieceOffset{ 1.065f },
		m_HoldingPiece{ false },
		m_MousePos{},
		m_Connector{},
		m_IsComputerTurn{ false },
		m_Turn{ 1 },
		m_ValidMoves{}
	{
		// Connect to Stockfish engine
		wchar_t path[] = L"resources/engines/stockfish_14_x64_avx2.exe";
		wchar_t path2[] = L"resources/engines/stockfish.exe";
		m_Connector.ConnectToEngine(path);

		LoadTextures();
		PrepareBoard();
		LoadPositions();
	}

	Board::~Board()
	{
		delete[] m_Board;
		delete[] m_Pieces;
	}

	void Board::Draw()
	{
		m_BoardTexture->Draw();

		for (int i = 0; i < TOTAL_PIECES; ++i)
			m_Pieces[i].Move(m_BoardOffset);

		for (int i = 0; i < TOTAL_PIECES; ++i)
		{
			m_Pieces[i].SetPosition(m_Pieces[i].GetPosition().x, m_Pieces[i].GetPosition().y, &m_PieceClip[i]);
			m_Pieces[i].Draw();
		}
		
		m_Pieces[m_PieceIndex].SetPosition(m_Pieces[m_PieceIndex].GetPosition().x, m_Pieces[m_PieceIndex].GetPosition().y, &m_PieceClip[m_PieceIndex]);
		m_Pieces[m_PieceIndex].Draw();

		for (int i = 0; i < TOTAL_PIECES; ++i)
			m_Pieces[i].Move(-m_BoardOffset);
	}

	bool Board::Update()
	{
		std::string fen{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
		
		// Computer move
		if (m_IsComputerTurn)
		{
			m_ValidMoves = m_Connector.GetValidMoves(fen);

			// print all moves
			for (const std::string& move : m_ValidMoves)
				std::cout << move << " \n";

			m_IsComputerTurn = false;
		}
		
		if (m_PrintMoves) // testing, need to do it once
		{
			m_ValidMoves = m_Connector.GetValidMoves(fen);
			
			// print all moves
			for (const std::string& move : m_ValidMoves)
				std::cout << move << " \n";

			m_PrintMoves = false;
		}

		// Dragging a piece
		if (m_IsMove) 
			m_Pieces[m_PieceIndex].SetPosition(m_MousePos.x - m_Dx, m_MousePos.y - m_Dy, &m_PieceClip[m_PieceIndex]);

		return true;
	}

	bool Board::HandleEvent(const SDL_Event& event)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_SPACE:
						m_IsComputerTurn = true;
						break;

					case SDLK_BACKSPACE:

						break;
				}
			} break;

			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);
				m_MousePos.x -= m_BoardOffset.x;
				m_MousePos.y -= m_BoardOffset.y;

				// Drag and drop
				if (event.button.button == SDL_BUTTON_LEFT)
					for (int i = 0; i < TOTAL_PIECES; i++)
					{
						SDL_Rect pieceBounds = m_Pieces[i].GetBounds();
						if (SDL_PointInRect(&m_MousePos, &pieceBounds) && !m_HoldingPiece)
						{
							m_IsMove = true;
							m_HoldingPiece = true;
							m_PieceIndex = i;

							m_Dx = m_MousePos.x - m_Pieces[i].GetPosition().x;
							m_Dy = m_MousePos.y - m_Pieces[i].GetPosition().y;

							m_OldPos = m_Pieces[i].GetPosition();
						}
					}
			} break;

			case SDL_MOUSEBUTTONUP:
			{
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);
				m_MousePos.x -= m_BoardOffset.x;
				m_MousePos.y -= m_BoardOffset.y;

				if (event.button.button == SDL_BUTTON_LEFT)
				{
					m_IsMove = false;
					m_HoldingPiece = false;
					Vector2f position = Vector2f(m_PieceSize / 2.0f, m_PieceSize / 2.0f) + m_Pieces[m_PieceIndex].GetPosition();
					m_NewPos = Vector2f(m_PieceSize * int(position.x / m_PieceSize) * m_PieceOffset, m_PieceSize * int(position.y / m_PieceSize) * m_PieceOffset);
					m_Str = ToChessNotation(m_OldPos) + ToChessNotation(m_NewPos);

					// Print move

					Move(m_Str); // remove piece

					std::cout << "[" << m_PieceIndex << "] " << "OldPos: " << "(" << std::ceil(m_Pieces[m_PieceIndex].GetPosition().x / (m_PieceOffset)) << ", " << std::ceil(m_Pieces[m_PieceIndex].GetPosition().y / (m_PieceOffset)) << ")\t" << "NewPos: " << "(" << m_NewPos.x << ", " << m_NewPos.y << ")\n";

					if (ToChessNotation(m_OldPos) != ToChessNotation(m_NewPos))
						m_PositionHistory += m_Str + " ";
					std::cout << m_PositionHistory << '\n';

					m_Pieces[m_PieceIndex].SetPosition(m_NewPos.x, m_NewPos.y, &m_PieceClip[m_PieceIndex]);
				}

			} break;
		}

		return true;
	}

	void Board::LoadPositions()
	{
		constexpr int RANK{ 8 };
		int pieceIndex{ 0 };

		for (int y = 0; y < RANK; y++)
			for (int x = 0; x < RANK; x++)
			{
				int boardSquare = m_Board[x + y * RANK];
				if (!boardSquare)
					continue;

				int xPos = abs(boardSquare) - 1;
				int yPos = (boardSquare > 0) ? 1 : 0;

				// Prepare piece clipping and position
				m_PieceClip[pieceIndex] = { m_PieceSize * xPos, m_PieceSize * yPos, m_PieceSize, m_PieceSize };
				m_Pieces[pieceIndex].SetPosition((m_PieceSize * x * m_PieceOffset), (m_PieceSize * y * m_PieceOffset), &m_PieceClip[m_PieceIndex]);
				++pieceIndex;
			}

		for (int i = 0; i < m_PositionHistory.length(); i += 5) // 5 because "d2d4" (for example) + whitespace
			Move(m_PositionHistory.substr(i, 4));
	}

	std::string Board::ToChessNotation(const Vector2f& position)
	{
		std::string notation{ "" };

		notation += char(m_PieceOffset * position.x / m_PieceSize + int('a'));
		notation += char(m_PieceOffset * 7 - position.y / m_PieceSize + int('1'));

		return notation;
	}

	Vector2f Board::ToCoord(char a, char b)
	{
		int x = int(a) - int('a');
		int y = 7 - int(b) + int('1');

		return Vector2f(x * m_PieceSize, y * m_PieceSize);
	}

	void Board::Move(const std::string& notation)
	{
		Vector2f OldPos = ToCoord(notation[0], notation[1]);
		Vector2f NewPos = ToCoord(notation[2], notation[3]);

		for (int i = 0; i < TOTAL_PIECES; i++)
			if (ToChessNotation(m_Pieces[i].GetPosition() / m_PieceOffset) == ToChessNotation(NewPos))
				m_Pieces[i].SetPosition(-100.0f, -100.0f, &m_PieceClip[i]);

		for (int i = 0; i < TOTAL_PIECES; i++)
			if (ToChessNotation(m_Pieces[i].GetPosition() / m_PieceOffset) == ToChessNotation(OldPos))
				m_Pieces[i].SetPosition(NewPos.x * m_PieceOffset, NewPos.y * m_PieceOffset, &m_PieceClip[i]);

		// Castling             // If king has not been moved
		if (notation == "e1g1") if (m_PositionHistory.find("e1") == std::string::npos) Move("h1f1");
		if (notation == "e8g8") if (m_PositionHistory.find("e8") == std::string::npos) Move("h8f8");
		if (notation == "e1c1") if (m_PositionHistory.find("e1") == std::string::npos) Move("a1d1");
		if (notation == "e8c8") if (m_PositionHistory.find("e8") == std::string::npos) Move("a8d8");
	}

	void Board::LoadTextures()
	{
		m_TextureHolder.Load(TextureID::Board, "resources/textures/0_DefaultBoard.png");
		m_TextureHolder.Load(TextureID::Pieces, "resources/textures/ChessPieces.png");
	}

	void Board::PrepareBoard()
	{
		m_BoardTexture = &m_TextureHolder.Get(TextureID::Board);
		m_BoardTexture->SetPosition(0, 0);

		for (int i = 0; i < TOTAL_PIECES; ++i)
			m_Pieces[i] = m_TextureHolder.Get(TextureID::Pieces);
	}
}

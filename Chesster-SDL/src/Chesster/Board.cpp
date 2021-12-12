#include "pch.h"
#include "Board.h"

#include "ImGuiWindows.h"

namespace Chesster
{
	constexpr int TOTAL_PIECES{ 32 };

	Board::Board(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_BoardTexture{ nullptr },
		m_Pieces{ new Texture[TOTAL_PIECES] },
		m_PieceSize{ 80 },
		m_IsDragging{ false },
		m_Dx{ 0.0f },
		m_Dy{ 0.0f },
		m_OldPos{ -100, -100 },
		m_NewPos{ -100, -100 },
		m_CurrentMove{ "" },
		m_PieceIndex{ 0 },
		m_MoveHistory{ "" },
		m_MoveHistorySize{ 0 },
		m_BoardOffset{ 22, 22 }, // 22, 23
		m_PieceOffset{ 1.063f, 1.063f }, // 1.063f, 1.063f
		m_HoldingPiece{ false },
		m_MousePos{},
		m_Connector{},
		m_IsComputerTurn{ false },
		m_IsAnimationDone{ true },
		m_FEN{},
		m_StartPosFEN{ "\"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"" },
		m_ValidMoves{},
		m_PathPythonScript{ "resources/engines/script/__init__.exe" }
	{
		// Connect to executables
		wchar_t path_Stockfish5[] = L"resources/engines/stockfish/stockfish_5.exe";
		wchar_t path_Stockfish14_avx2[] = L"resources/engines/stockfish/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe";
		wchar_t path_Stockfish14_popcnt[] = L"resources/engines/stockfish/stockfish_14.1_win_x64_popcnt/stockfish_14.1_win_x64_popcnt.exe";
		wchar_t path_Hannibal_x64[] = L"resources/engines/Hannibal1.7/Hannibal1.7x64.exe";
		
		m_Connector.ConnectToEngine(path_Stockfish14_avx2);

		m_ValidMoves = m_Connector.GetValidMoves(m_PathPythonScript, m_StartPosFEN);
		m_FEN += "\"" + m_Connector.GetFEN(" ") + "\"";

		// Assign textures
		m_BoardTexture = &context.textures->Get(TextureID::Board);
		m_BoardTexture->SetPosition(0, 0);

		for (int i = 0; i < TOTAL_PIECES; ++i)
			m_Pieces[i] = context.textures->Get(TextureID::Pieces);

		LoadPositions();
	}

	bool Board::HandleEvent(SDL_Event& event)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					// Allows the computer to make a move
					case SDLK_SPACE:
						m_IsComputerTurn = true;
						break;

					// Go backwards one move
					case SDLK_BACKSPACE:
					{
						int i{ 5 }; // char quantity to erase. notation + white space
						if (m_MoveHistory.length() > 6)
							m_MoveHistory.erase(m_MoveHistory.length() - 6, i);

						LoadPositions();
					} break;
				}
			} break;
			
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			{
				// Get the mouse screen coordinates
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);
				m_MousePos.x -= m_BoardOffset.x;
				m_MousePos.y -= m_BoardOffset.y;

				// Drag and drop
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					for (int i = 0; i < TOTAL_PIECES; ++i)
					{
						SDL_Rect pieceBounds = m_Pieces[i].GetBounds();
						if (SDL_PointInRect(&m_MousePos, &pieceBounds) && !m_HoldingPiece)
						{
							m_IsDragging = true;
							m_HoldingPiece = true;
							m_PieceIndex = i;

							m_Dx = m_MousePos.x - m_Pieces[i].GetPosition().x;
							m_Dy = m_MousePos.y - m_Pieces[i].GetPosition().y;

							m_OldPos = Vector2i(m_PieceSize * (m_Pieces[i].GetPosition().x / m_PieceSize), m_PieceSize * (m_Pieces[i].GetPosition().y / m_PieceSize));
						}
					}
				}
			} break;
			
			// Mouse button released
			case SDL_MOUSEBUTTONUP:
			{
				// Get the mouse screen coordinates and adjust for offset
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);
				m_MousePos.x -= m_BoardOffset.x;
				m_MousePos.y -= m_BoardOffset.y;

				// Left mouse button
				if (event.button.button == SDL_BUTTON_LEFT && !m_Promoting)
				{
					m_IsDragging = false;
					Vector2i position = m_Pieces[m_PieceIndex].GetPosition() + Vector2i(m_PieceSize / 2, m_PieceSize / 2);
					m_NewPos = Vector2i(m_PieceSize * (position.x / m_PieceSize), m_PieceSize * (position.y / m_PieceSize));
					m_CurrentMove = ToChessNotation(m_OldPos) + ToChessNotation(m_NewPos);

					// Check if player is doing a pawn promotion
					if (IsWhitePawn(m_PieceIndex) && IsRow(ToChessNotation(m_NewPos), '8') && IsRow(ToChessNotation(m_OldPos), '7') ||
						IsBlackPawn(m_PieceIndex) && IsRow(ToChessNotation(m_NewPos), '1') && IsRow(ToChessNotation(m_OldPos), '2'))
					{
						RequestStackPush(StateID::PawnPromo);
						m_Promoting = true;
					}

					// Check the list of valid moves
					ValidateMove();

					m_HoldingPiece = false;
				}

			} break;
		}

		return true;
	}

	bool Board::Update(const std::chrono::duration<double>& dt)
	{
		// Computer move
		if (m_IsComputerTurn)
		{
			// Get stockfish's next move
			m_CurrentMove = m_Connector.GetNextMove(m_MoveHistory);
			if (m_CurrentMove == "error")
				return false;

			m_OldPos = ToCoord(m_CurrentMove[0], m_CurrentMove[1]);
			m_NewPos = ToCoord(m_CurrentMove[2], m_CurrentMove[3]);

			// If a piece on the board has the notation, move it
			for (int i = 0; i < TOTAL_PIECES; ++i)
				if (ToChessNotation(m_Pieces[i].GetPosition()) == ToChessNotation(m_OldPos))
					m_PieceIndex = i;

			/* Could do animation here */
			{
			}

			// Remove any piece it "ate", update move history and piece position
			Move(m_CurrentMove);
			m_MoveHistory += m_CurrentMove + " ";

			m_Pieces[m_PieceIndex].SetPosition(m_NewPos.x * m_PieceOffset.x, m_NewPos.y * m_PieceOffset.y, &m_PieceClip[m_PieceIndex]);

			m_IsComputerTurn = false;
		}

		// Dragging a piece
		if (m_IsDragging)
			m_Pieces[m_PieceIndex].SetPosition(m_MousePos.x - m_Dx, m_MousePos.y - m_Dy, &m_PieceClip[m_PieceIndex]);

		// Check if a move was played successfully
		if (m_MoveHistory.size() != m_MoveHistorySize)
		{
			// Update move count
			m_MoveHistorySize = m_MoveHistory.size();

			// Get FEN string then get updated valid moves list
			m_FEN.clear();
			m_FEN += "\"" + m_Connector.GetFEN(m_MoveHistory) + "\"";

			m_ValidMoves = m_Connector.GetValidMoves(m_PathPythonScript, m_FEN);

			m_WinningColor = !m_WinningColor; // TODO: need a better way of checking who won
		}

		// Update pawn promotion selection
		if (m_Promoting)
		{
			m_CurrentMove.push_back(*PawnPromotionState::GetPiecePromo());
			*PawnPromotionState::GetPiecePromo() = 'x';
			ValidateMove();
			m_Promoting = false;
		}

		return true;
	}

	void Board::Draw()
	{
		m_BoardTexture->Draw();

		PaintActiveSquares();

		for (int i = 0; i < TOTAL_PIECES; ++i)
			m_Pieces[i].Move(m_BoardOffset);

		// Draw all pieces
		for (int i = 0; i < TOTAL_PIECES; ++i)
		{
			m_Pieces[i].SetPosition(m_Pieces[i].GetPosition().x, m_Pieces[i].GetPosition().y, &m_PieceClip[i]);
			m_Pieces[i].Draw();
		}

		// Draw selected piece on top of all others
		m_Pieces[m_PieceIndex].SetPosition(m_Pieces[m_PieceIndex].GetPosition().x, m_Pieces[m_PieceIndex].GetPosition().y, &m_PieceClip[m_PieceIndex]);
		m_Pieces[m_PieceIndex].Draw();

		for (int i = 0; i < TOTAL_PIECES; ++i)
			m_Pieces[i].Move(-m_BoardOffset);
	}

	void Board::ResetBoard()
	{
		m_OldPos = { -100, -100 },
		m_NewPos = { -100, -100 },

		WhitePawns = { 16, 17, 18, 19, 20, 21, 22, 23 };
		BlackPawns = { 8, 9, 10, 11, 12, 13, 14, 15 };

		m_MoveHistory.clear();
		m_MoveHistorySize = m_MoveHistory.size();

		LoadPositions();
		m_Connector.ResetGame();
		m_ValidMoves = m_Connector.GetValidMoves(m_PathPythonScript, m_StartPosFEN);

	}

	void Board::EvaluateBoard()
	{
		m_Connector.EvaluateGame();
	}

	void Board::ChangeDifficulty()
	{
		m_Connector.SetDifficulty(AppSettingsGUI::m_Difficulty);
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
				m_Pieces[pieceIndex].SetPosition((m_PieceSize * x * m_PieceOffset.x), (m_PieceSize * y * m_PieceOffset.y), &m_PieceClip[m_PieceIndex]);
				++pieceIndex;
			}

		for (int i = 0; i < m_MoveHistory.length(); i += 5) // 5 because "d2d4" (for example) + whitespace
			Move(m_MoveHistory.substr(i, 4));
	}

	std::string Board::ToChessNotation(const Vector2i& position)
	{
		std::string notation{ "" };

		notation += char(m_PieceOffset.x * position.x / m_PieceSize + int('a'));
		notation += char(m_PieceOffset.y * 7 - position.y / m_PieceSize + int('1'));

		return notation;
	}

	Vector2i Board::ToCoord(char a, char b)
	{
		int x = int(a) - int('a');
		int y = 7 - int(b) + int('1'); // 7 cuz it's from 0 to 7 (8 files)

		return Vector2i(x * m_PieceSize, y * m_PieceSize);
	}

	bool Board::IsWhitePawn(const int& index)
	{
		for (const int& i : WhitePawns)
			if (i == index)
				return true;

		return false;
	}

	bool Board::IsBlackPawn(const int& index)
	{
		for (const int& i : BlackPawns)
			if (i == index)
				return true;

		return false;
	}

	bool Board::IsPawn(const int& index)
	{
		return { IsWhitePawn(index) || IsBlackPawn(index) };
	}

	bool Board::IsRow(const std::string& notation, char row)
	{
		if (notation.back() == row)
			return true;

		return false;
	}

	void Board::CheckPawnPromotion(int offset)
	{
		char c;
		if (offset > 0) c = '8';
		else c = '1';

		if (IsRow(ToChessNotation(m_Pieces[m_PieceIndex].GetPosition()), c))
		{
			char piece{ m_CurrentMove.back() };
			switch (piece)
			{
				case 'n': // knight
				{
					if (offset > 0) // is white
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 1, m_PieceSize * 1, m_PieceSize, m_PieceSize };
						ErasePawn(WhitePawns);
					}
					else // is black
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 1, m_PieceSize * 0, m_PieceSize, m_PieceSize };
						ErasePawn(BlackPawns);
					}
				} break;

				case 'b': // bishop
				{
					if (offset > 0)
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 2, m_PieceSize * 1, m_PieceSize, m_PieceSize };
						ErasePawn(WhitePawns);
					}
					else
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 2, m_PieceSize * 0, m_PieceSize, m_PieceSize };
						ErasePawn(BlackPawns);
					}
				} break;

				case 'r': // rook
				{
					if (offset > 0)
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 0, m_PieceSize * 1, m_PieceSize, m_PieceSize };
						ErasePawn(WhitePawns);
					}
					else
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 0, m_PieceSize * 0, m_PieceSize, m_PieceSize };
						ErasePawn(BlackPawns);
					}
				} break;

				case 'q': // queen
				{
					if (offset > 0)
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 3, m_PieceSize * 1, m_PieceSize, m_PieceSize };
						ErasePawn(WhitePawns);
					}
					else
					{
						m_PieceClip[m_PieceIndex] = { m_PieceSize * 3, m_PieceSize * 0, m_PieceSize, m_PieceSize };
						ErasePawn(BlackPawns);
					}
				} break;

				default:
					CHESSTER_ERROR("Invalid 5th char");
					break;
			}
		}
	}

	void Board::ErasePawn(std::vector<int>& pawns)
	{
		for (auto itr = pawns.begin(); itr != pawns.end(); )
		{
			if (*itr == m_PieceIndex)
				pawns.erase(itr);
			else
				++itr;
		}
	}

	void Board::Move(const std::string& notation)
	{
		Vector2i OldPos = ToCoord(notation[0], notation[1]);
		Vector2i NewPos = ToCoord(notation[2], notation[3]);

		// Deal with "eaten" pieces, moves them offscreen
		for (int i = 0; i < TOTAL_PIECES; ++i)
		{
			if (ToChessNotation(m_Pieces[i].GetPosition()) == ToChessNotation(NewPos)
				&& i != m_PieceIndex) // don't count itself
				m_Pieces[i].SetPosition(-100.0f, -100.0f, &m_PieceClip[i]);
		}
		
		// Move rooks when castling
		for (int i = 0; i < TOTAL_PIECES; ++i)
			if (ToChessNotation(m_Pieces[i].GetPosition()) == ToChessNotation(OldPos))
				m_Pieces[i].SetPosition(NewPos.x * m_PieceOffset.x, NewPos.y * m_PieceOffset.y, &m_PieceClip[i]);

		// Castling             // If king has not been moved					   // Move Rook
		if (notation == "e1g1") if (m_MoveHistory.find("e1") == std::string::npos) Move("h1f1");
		if (notation == "e8g8") if (m_MoveHistory.find("e8") == std::string::npos) Move("h8f8");
		if (notation == "e1c1") if (m_MoveHistory.find("e1") == std::string::npos) Move("a1d1");
		if (notation == "e8c8") if (m_MoveHistory.find("e8") == std::string::npos) Move("a8d8");

		// En passant
		int offset{ m_PieceSize };
		if (IsBlackPawn(m_PieceIndex)) offset = -m_PieceSize;

		bool isPawn{ false };
		if (IsPawn(m_PieceIndex))
		{
			isPawn = true;
			// Iterate all 32 pieces to find if a pawn was eaten
			for (int i = 0; i < TOTAL_PIECES; ++i)
			{
				// If a piece is one square behind then it's the pawn that was eaten
				if (ToChessNotation(Vector2i(m_Pieces[m_PieceIndex].GetPosition().x, m_Pieces[m_PieceIndex].GetPosition().y + offset))
					== ToChessNotation(m_Pieces[i].GetPosition()))
				{
					// Safeguard to only eat the correct pawn
					if (IsBlackPawn(m_PieceIndex) && !IsBlackPawn(i) && IsRow(ToChessNotation(m_Pieces[m_PieceIndex].GetPosition()), '3') ||
						IsWhitePawn(m_PieceIndex) && !IsWhitePawn(i) && IsRow(ToChessNotation(m_Pieces[m_PieceIndex].GetPosition()), '6'))
						m_Pieces[i].SetPosition(-100.0f, -100.0f, &m_PieceClip[i]);
				}
			}
		}

		// Pawn promotions
		if (isPawn)
			CheckPawnPromotion(offset);
	}

	void Board::ValidateMove()
	{
		for (const std::string& move : m_ValidMoves)
		{
			if (move.c_str() == m_CurrentMove)
			{
				Move(m_CurrentMove);

				// Update move history if piece is dropped in a new square
				if (ToChessNotation(m_OldPos) != ToChessNotation(m_NewPos))
					m_MoveHistory += m_CurrentMove + " ";
				++m_MoveHistorySize;

				m_Pieces[m_PieceIndex].SetPosition(m_NewPos.x * m_PieceOffset.x, m_NewPos.y * m_PieceOffset.y, &m_PieceClip[m_PieceIndex]);
				break;
			}
			// If not a valid move return piece to original position
			else if ((move.c_str() != m_CurrentMove) && m_HoldingPiece)
				m_Pieces[m_PieceIndex].SetPosition(m_OldPos.x * m_PieceOffset.x, m_OldPos.y * m_PieceOffset.y, &m_PieceClip[m_PieceIndex]);
		}
	}

	void Board::PaintActiveSquares()
	{
		int posOffset{ 3 }, sizeOffset{ 5 };

		m_HighlightedSquares[0] = { (m_OldPos.x + m_BoardOffset.x - posOffset) * m_PieceOffset.x, (m_OldPos.y  + m_BoardOffset.y - posOffset) * m_PieceOffset.y,
								(float)m_PieceSize + sizeOffset, (float)m_PieceSize + sizeOffset };

		m_HighlightedSquares[1] = { (m_NewPos.x + m_BoardOffset.x - posOffset) * m_PieceOffset.x, (m_NewPos.y + m_BoardOffset.y - posOffset) * m_PieceOffset.y,
								(float)m_PieceSize + sizeOffset, (float)m_PieceSize + sizeOffset };

		SDL_SetRenderDrawColor(Window::Renderer, 200u, 200u, 0u, 100u);
		SDL_RenderFillRectF(Window::Renderer, &m_HighlightedSquares[0]);
		SDL_SetRenderDrawColor(Window::Renderer, 100u, 100u, 0u, 100u);
		SDL_RenderFillRectF(Window::Renderer, &m_HighlightedSquares[1]);
		SDL_SetRenderDrawColor(Window::Renderer, 21u, 21u, 255u, 255u);
	}
}

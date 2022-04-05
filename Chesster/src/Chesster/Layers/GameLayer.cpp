#include "pch.h"
#include "Chesster/Layers/GameLayer.h"

#include "Chesster/Core/Application.h"
#include "Chesster/Renderer/Renderer.h"

namespace Chesster
{
	static bool s_IsEngineThreadRunning{ true };
	
	GameLayer* GameLayer::s_Instance{ nullptr };

	Network GameLayer::s_TCPConnection{};

	GameLayer::GameLayer() :
		Layer("GameLayer"),
		m_StartPosFEN{ "\"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"" },
		m_PathPythonScript{ "assets/script/__init__.exe" }
	{
		s_Instance = this;

		unsigned threadID{};
		_beginthreadex(nullptr, 0, &EngineThread, (void*)this, 0, &threadID);
	}

	void GameLayer::OnAttach()
	{
		// Frambuffer init
		Window& window = Application::Get().GetWindow();
		m_Framebuffer = std::make_unique<Framebuffer>(window.GetWidth(), window.GetHeight());

		// Board init
		m_Board = Board(glm::vec2(m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight()));

		// Pieces init
		m_PieceTexture = std::make_unique<Texture>("assets/textures/ChessPieces.png");
		m_PieceTexture->SetWidth(m_Pieces[0].Size);
		m_PieceTexture->SetHeight(m_Pieces[0].Size);
		SetPieceClips();
		ResetPieces();
	}

	void GameLayer::OnDetach()
	{
		s_IsEngineThreadRunning = false;
	}

	void GameLayer::OnEvent(SDL_Event& sdlEvent)
	{
		if (m_CurrentGameState == GameState::Gameplay)
		{
			switch (sdlEvent.type)
			{
				case SDL_KEYDOWN:
				{
					if (sdlEvent.key.keysym.sym == SDLK_F4 && sdlEvent.key.repeat == 0 && !m_IsComputerTurn && !m_IsRecvComputerMove)
					{
						//s_TCPConnection.SendCommand();
						m_IsComputerTurn = true;
						Sleep(150);
					}
					break;
				}

				case SDL_MOUSEMOTION:
				{
					int MouseX{ 0 }, MouseY{ 0 };
					SDL_GetMouseState(&MouseX, &MouseY);
					m_MouseCoords = { MouseX, MouseY };

					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if (sdlEvent.button.button == SDL_BUTTON_LEFT && !m_IsComputerTurn && !m_IsRecvComputerMove)
					{
						for (const Piece& piece : m_Pieces)
						{
							if (IsPointInRect(m_ViewportMousePos, piece.WorldBounds) && !m_IsHoldingPiece)
							{
								m_PieceIndex = piece.Index;
								m_IsHoldingPiece = true;
							}
						}
					}
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					if (sdlEvent.button.button == SDL_BUTTON_LEFT && m_IsHoldingPiece)
					{
						m_IsPieceReleased = true;
						m_IsHoldingPiece = false;
					}
					break;
				}
			}
		}
	}

	void GameLayer::OnUpdate(const std::chrono::duration<double>& dt)
	{
		// On ImGui window resize
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(m_Framebuffer->GetWidth() != m_ViewportSize.x || m_Framebuffer->GetHeight() != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Board.OnViewportResize({ m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight() });

			for (auto& p : m_Pieces) p.OnViewportResize();
		}

		// Chess engine's move
		if (m_IsRecvComputerMove)
			UpdateComputerMove();

		// Dragging a piece
		if (m_IsHoldingPiece)
			m_Pieces[m_PieceIndex].SetPosition(m_ViewportMousePos.x, m_ViewportMousePos.y);

		// Player released piece held by mouse click
		if (m_IsPieceReleased)
		{
			m_IsPieceReleased = false;
			UpdatePlayerMove();

			// Guard against out of board placement
			if (IsNotationValid(m_Pieces[m_PieceIndex].Notation))
				m_Pieces[m_PieceIndex].SetPosition(m_TargetSquare.Center.x, m_TargetSquare.Center.y);
		}

		// A new move has been played
		if (m_MoveHistorySize != m_MoveHistory.size())
		{
			m_Board.OnNewMove(m_Pieces, m_CurrentMove, m_PieceIndex);

			m_MoveHistorySize = m_MoveHistory.size();
			m_IsMovePlayed = true;

			// Update current player
			++m_CurrentPlayer;
		}

		// Interface Buttons
		if (ConsoleButtons::ResetBoardButton)
		{
			ResetBoard();
			ConsoleButtons::ResetBoardButton = false;
		}

		if (ConsoleButtons::EvaluateBoardButton)
		{
			EvaluateBoard();
			ConsoleButtons::EvaluateBoardButton = false;
		}

		// Check for New Game (take pic and compare, etc)
		//m_NewGameData = s_TCPConnection.GetCameraData();

		m_Board.OnUpdate(dt);
	}

	void GameLayer::OnRender()
	{
		// Render to ImGui's viewport window
		m_Framebuffer->Bind();

		Renderer::SetClearColor(m_SettingsPanel.GetClearColor());
		Renderer::Clear();

		// Draw all the chess board squares
		m_Board.OnRender();
		
		// Draw all the chess pieces
		for (auto& piece : m_Pieces)
		{
			m_PieceTexture->SetClip(&piece.m_TextureClip);
			m_PieceTexture->SetPosition(piece.Position.x, piece.Position.y);
			Renderer::DrawTexture(m_PieceTexture.get());
		}

		// Draw the selected chess piece on top of all other chess pieces
		m_PieceTexture->SetClip(&m_Pieces[m_PieceIndex].m_TextureClip);
		m_PieceTexture->SetPosition(m_Pieces[m_PieceIndex].Position.x, m_Pieces[m_PieceIndex].Position.y);
		Renderer::DrawTexture(m_PieceTexture.get());

		// Draw a faded black screen when gameover
		if (m_CurrentGameState == GameState::Gameover)
		{
			const SDL_Rect blackOverlayRect = { 0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight() };
			const glm::vec4 blackOverlayColor = { 0, 0, 0, 150 };
			Renderer::DrawFilledRect(blackOverlayRect, blackOverlayColor);
		}

		m_Framebuffer->Unbind();
	}

	void GameLayer::OnImGuiRender()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));		
		ImGui::Begin("DockSpace", (bool*)0, windowFlags);
		ImGui::PopStyleVar(3);

		// Set minimum window size
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.0f;

		// DockSpace
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		style.WindowMinSize.x = minWinSizeX;

		// Settings Panel
		//SettingsPanel::OnImGuiRender();
		m_SettingsPanel.OnImGuiRender();

		// Console Panel
		s_ConsolePanel.OnImGuiRender();

		/////////////////////////////////////////////////////////////////////////////////////////////
		//// Viewport Window ////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_ViewportMousePos.x = m_MouseCoords.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
		m_ViewportMousePos.y = m_MouseCoords.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		SDL_Texture* textureID = m_Framebuffer->GetSDLTexture();
		ImGui::Image((ImTextureID*)(intptr_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y });

		// Gameover window
		if (m_CurrentGameState == GameState::Gameover)
			GameoverPopup();

		// Pawn promotion prompt
		if (m_CurrentGameState == GameState::PawnPromotion)
			PawnPromotionPopup();

		ImGui::End(); // End "Viewport"
		ImGui::PopStyleVar();

		ImGui::End(); // End "DockSpace"
	}

	void GameLayer::UpdateComputerMove()
	{
		s_ConsolePanel.AddLog("Computer is playing...");
		LOG_INFO("Computer is playing...");

		// Grab the selected chess piece's old and new positions
		std::string oldPos{ m_CurrentMove[0], m_CurrentMove[1] };
		std::string newPos{ m_CurrentMove[2], m_CurrentMove[3] };

		auto& squaresMap = m_Board.GetSquaresMap();
		auto oldSquare = squaresMap.find(oldPos);
		auto newSquare = squaresMap.find(newPos);
		if (oldSquare != squaresMap.end() && newSquare != squaresMap.end())
		{
			// Iterate all 32 pieces to find the matching one
			for (Piece& piece : m_Pieces)
			{
				if (piece.Notation == oldSquare->second->Notation)
				{
					m_PieceIndex = piece.Index;
					m_TargetSquare = *newSquare->second;
					break;
				}
			}

			// Pawn Promotions
			if (m_Pieces[m_PieceIndex].IsPromotion(m_CurrentMove))
				m_Pieces[m_PieceIndex].UpdateTextureClip(m_CurrentMove);
			
			// Update chess piece
			m_Pieces[m_PieceIndex].SetPosition(m_TargetSquare.Center.x, m_TargetSquare.Center.y);
			m_Pieces[m_PieceIndex].Notation = m_TargetSquare.Notation;
			m_Pieces[m_PieceIndex].CheckEnPassant(oldPos);
			m_MoveHistory += m_CurrentMove + ' ';
			s_ConsolePanel.AddLog(std::string("Computer moved: " + m_CurrentMove).c_str());

			// Remove captured pieces
			for (Piece& piece : m_Pieces)
				if (m_Pieces[m_PieceIndex].Notation == piece.Notation && !(m_Pieces[m_PieceIndex].Index == piece.Index))
					RemovePiece(piece);
		}

		m_IsRecvComputerMove = false;
	}

	void GameLayer::UpdatePlayerMove()
	{
		// Find the square where the piece was dropped at
		for (const Board::Square& square : m_Board.GetBoardSquares())
		{
			if (IsPointInRect(m_ViewportMousePos, square.WorldBounds))
			{
				// If piece was not moved to a new square
				if (m_Pieces[m_PieceIndex].Notation == square.Notation)
				{
					m_Pieces[m_PieceIndex].SetPosition(square.Center.x, square.Center.y);
					break;
				}

				m_CurrentMove = { m_Pieces[m_PieceIndex].Notation + square.Notation };

				// Pawn Promotions
				if (m_Pieces[m_PieceIndex].IsPromotion(m_CurrentMove))
					m_CurrentGameState = GameState::PawnPromotion;

				if (m_CurrentGameState == GameState::Gameplay)
				{
					if (IsCurrentMoveLegal())
					{
						// Update position
						m_Pieces[m_PieceIndex].SetPosition(square.Center.x, square.Center.y);
						std::string oldPos = m_Pieces[m_PieceIndex].Notation;
						m_Pieces[m_PieceIndex].Notation = square.Notation;
						m_Pieces[m_PieceIndex].CheckEnPassant(oldPos);

						// Capture a piece (if any)
						for (Piece& piece : m_Pieces)
							if (m_Pieces[m_PieceIndex].Notation == piece.Notation &&
								!(m_Pieces[m_PieceIndex].Index == piece.Index))
								RemovePiece(piece);

						// Update move history
						m_MoveHistory += m_CurrentMove + ' ';

						std::string msg{ "Player move: " + m_CurrentMove };
						LOG_INFO(msg);
						s_ConsolePanel.AddLog(msg.c_str());

						// Update states
						m_IsPlayerPlayed = true;
						break;
					}
					else
					{
						// Illegal move, return piece to original position
						if (IsNotationValid(m_Pieces[m_PieceIndex].Notation))
						{
							m_Pieces[m_PieceIndex].SetPosition(m_TargetSquare.Center.x, m_TargetSquare.Center.y);
							s_ConsolePanel.AddLog(" Wait... that's illegal!\n");
							LOG_ERROR("Wait... that's illegal!");
							break;
						}
					}
				}
			}
		}
	}

	void GameLayer::SetPieceClips()
	{
		int* pieceLocations = new int[8 * 8]
		{
			-1, -2, -3, -4, -5, -3, -2, -1,
			-6, -6, -6, -6, -6, -6, -6, -6,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 6,  6,  6,  6,  6,  6,  6,  6,
			 1,  2,  3,  4,  5,  3,  2,  1
		};

		constexpr int RANK{ 8 };
		constexpr int SIZE{ 80 };

		int pieceIndex{ 0 };
		for (int y = 0; y < RANK; y++)
		{
			for (int x = 0; x < RANK; x++)
			{
				int boardSquare = pieceLocations[x + y * RANK];
				if (!boardSquare)
					continue;

				int xPos = abs(boardSquare) - 1;
				int yPos = (boardSquare > 0) ? 1 : 0;

				// Prepare piece clipping and position
				m_Pieces[pieceIndex].m_TextureClip = { SIZE * xPos, SIZE * yPos, SIZE, SIZE };
				++pieceIndex;
			}
		}

		delete[] pieceLocations;
	}

	void GameLayer::RemovePiece(Piece& piece)
	{
		std::string capturedAtNotation = piece.Notation;

		piece.Notation = "00";
		piece.SetPosition(-3000.0f, -3000.0f);
		piece.IsCaptured = true;
	}

	void GameLayer::ResetPieces()
	{
		// Set piece starting positions and properties
		size_t i{ 0 };
		uint32_t offset{ 0 };
		PieceColor color = PieceColor::Black;
		for (Piece& piece : m_Pieces)
		{
			if (i > 15) { offset = 32; color = PieceColor::White; }
			auto& squares = m_Board.GetBoardSquares();

			// Set up the piece properties
			piece.SetPosition(squares[i + offset].Center.x, squares[i + offset].Center.y);
			piece.Notation = squares[i + offset].Notation;
			piece.Index = i;
			piece.SetType();
			piece.Color = color;
			piece.EnPassant = false;
			piece.IsCaptured = false;
			++i;
		}
	}

	void GameLayer::PromotePawn()
	{
		std::string move = { m_CurrentMove[2], m_CurrentMove[3] };
		auto& x = m_Board.m_SquaresMap.find(move);
		if (x != m_Board.m_SquaresMap.end())
		{
			m_Pieces[m_PieceIndex].UpdateTextureClip(m_CurrentMove);
			m_Pieces[m_PieceIndex].SetPosition(x->second->Center.x, x->second->Center.y);
			m_Pieces[m_PieceIndex].Notation = x->second->Notation;

			// Capture a piece (if any)
			for (Piece& piece : m_Pieces)
				if (m_Pieces[m_PieceIndex].Notation == piece.Notation &&
					!(m_Pieces[m_PieceIndex].Index == piece.Index))
					RemovePiece(piece);

			// Update move history
			m_MoveHistory += m_CurrentMove + ' ';

			// Print message to consoles
			s_ConsolePanel.AddLog("Player move: %s", m_CurrentMove);
			LOG_INFO(std::string("Player move: " + m_CurrentMove).c_str());

			m_IsPlayerPlayed = true;
		}
	}

	void GameLayer::ResetBoard()
	{
		m_CurrentPlayer = Player::White;
		m_CurrentGameState = GameState::Gameplay;

		//m_CameraOldData.clear();
		//m_CameraNewData.clear();

		m_PieceIndex = 0;
		m_CurrentMove.clear();
		m_MoveHistory.clear();
		m_MoveHistorySize = m_MoveHistory.size();
		m_Board.Reset();

		SetPieceClips();
		ResetPieces();
		m_Connector.ResetGame();
		m_LegalMoves = m_Connector.GetValidMoves(m_PathPythonScript, m_StartPosFEN);

		m_TempData.clear();
		m_OldData.clear();
		m_NewData.clear();
		m_NewGameData.clear();
	}

	void GameLayer::EvaluateBoard()
	{
		m_Connector.EvaluateGame();
	}

	bool GameLayer::IsPointInRect(const glm::vec2& point, const RectBounds& rect)
	{
		return ((point.x >= rect.left) && (point.x <= rect.right) &&
			(point.y >= rect.bottom) && (point.y <= rect.top)) ? true : false;
	}

	bool GameLayer::IsNotationValid(const std::string& notation)
	{
		auto& squaresMap = m_Board.GetSquaresMap();
		auto x = squaresMap.find(notation);
		if (x != squaresMap.end())
		{
			m_TargetSquare = *x->second;
			return true;
		}

		return false;
	}

	bool GameLayer::IsCurrentMoveLegal()
	{
		// Compare current move to all the legal moves
		for (const std::string& move : m_LegalMoves)
			if (m_CurrentMove == move)
				return true;

		return false;
	}

	void GameLayer::GameoverPopup()
	{
		ImGui::SetNextWindowPos({ (m_ViewportSize.x / 2.0f) - 100, m_ViewportSize.y / 2.0f });
		ImGui::SetNextWindowSize({ 200, 80 });
		ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		ImGui::Begin("Gameover", nullptr, winFlags);

		std::string winner{};
		if (m_CurrentPlayer == Player::Black)
			winner = { "White Won!" };
		else
			winner = { "Black Won!" };

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[2];

		ImGui::PushFont(boldFont);
		ImGui::Text(winner.c_str());
		if (ImGui::Button("Play Again", { 200, 50 }))
			ResetBoard();
		ImGui::PopFont();

		ImGui::End();
	}

	void GameLayer::PawnPromotionPopup()
	{
		ImGui::SetNextWindowPos({ (m_ViewportSize.x / 2.0f) - 100, m_ViewportSize.y / 2.0f });
		ImGui::SetNextWindowSize({ 200, 120 });
		ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		ImGui::Begin("PawnPromo", nullptr, winFlags);

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[2];

		ImGui::PushFont(boldFont);
		ImGui::Text("Select piece:");
		if (ImGui::Button("Knight", { 90, 40 }))
		{
			m_CurrentMove.push_back('n');
			PromotePawn();
			m_CurrentGameState = GameState::Gameplay();
		}
		ImGui::SameLine();
		if (ImGui::Button("Bishop", { 90, 40 }))
		{
			m_CurrentMove.push_back('b');
			PromotePawn();
			m_CurrentGameState = GameState::Gameplay();
		}
		if (ImGui::Button("Rook", { 90, 40 }))
		{
			m_CurrentMove.push_back('r');
			PromotePawn();
			m_CurrentGameState = GameState::Gameplay();

		}
		ImGui::SameLine();
		if (ImGui::Button("Queen", { 90, 40 }))
		{
			m_CurrentMove.push_back('q');
			PromotePawn();
			m_CurrentGameState = GameState::Gameplay();
		}
		ImGui::PopFont();

		ImGui::End();
	}

	unsigned int __stdcall GameLayer::EngineThread(void* data)
	{
		GameLayer* Game = static_cast<GameLayer*>(data);

		wchar_t path_Stockfish14_avx2[] = L"assets/engines/stockfish/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe";
		Game->m_Connector.ConnectToEngine(path_Stockfish14_avx2);

		Game->m_LegalMoves = Game->m_Connector.GetValidMoves(Game->m_PathPythonScript, Game->m_StartPosFEN);
		Game->m_CurrentFEN = { "\"" + Game->m_Connector.GetFEN(" ") + "\"" };

		while (s_IsEngineThreadRunning)
		{
			if (Game->m_IsMovePlayed && !Game->m_IsComputerTurn)
			{
				// Update legal moves list
				Game->m_CurrentFEN = { "\"" + Game->m_Connector.GetFEN(Game->m_MoveHistory) + "\"" };
				Game->m_LegalMoves = Game->m_Connector.GetValidMoves(Game->m_PathPythonScript, Game->m_CurrentFEN);
				
				if (Game->m_LegalMoves.empty())
					Game->m_CurrentGameState = GameState::Gameover;

				// Make computer play automatically after player
				if (Game->m_IsPlayerPlayed)
				{
					//game->m_IsComputerTurn = true;
					Game->m_IsPlayerPlayed = false;
				}

				Game->m_IsMovePlayed = false;
			}

			if (Game->m_IsComputerTurn)
			{
				// Get computer's move
				Game->m_CurrentMove = Game->m_Connector.GetNextMove(Game->m_MoveHistory);
				if (Game->m_CurrentMove == "error")
				{
					Game->s_ConsolePanel.AddLog("Failed to get engine move.");
					Game->s_ConsolePanel.AddLog("Enter <spacebar> to try again.");
					LOG_ERROR("Failed to get engine move.");
					Game->m_IsRecvComputerMove = false;
				}
				else
				{
					Game->m_IsRecvComputerMove = true;
					Game->m_IsComputerTurn = false;
				}
			}
		}

		return 100;
	}

	GameLayer::Player operator++(GameLayer::Player& player)
	{
		return player =
		{
			(player == GameLayer::Player::White) ?
			GameLayer::Player::Black : GameLayer::Player::White
		};
	}
}

#include "pch.h"
#include "Chesster/Layers/GameLayer.h"

#include "Chesster/Core/Application.h"

#include "Chesster/Connections/ChessEngine.h"

#include <imgui.h>

namespace Chesster
{
	// Boolean switches
	static bool s_IsEngineThreadRunning{ true };
	static bool s_IsComputerTurn{ false };
	static bool s_IsMovePlayed{ false };
	static bool s_IsHoldingPiece{ false };
	
	GameLayer* GameLayer::s_Instance{ nullptr };

	GameLayer::GameLayer() :
		Layer("GameLayer"),
		m_ChessEngine{ nullptr },
		m_ViewportSize{ 0.0f, 0.0f },
		m_MousePos{ 0.0f, 0.0f },
		m_ViewportMousePos{ 0.0f, 0.0f },
		m_StartPosFEN{ "\"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"" },
		m_CurrentPiece{ &Piece{} }
	{
		s_Instance = this;

		m_ChessEngine = std::make_unique<ChessEngine>();

		unsigned threadID{};
		_beginthreadex(nullptr, 0, &ChessEngineThread, (void*)this, 0, &threadID);
	}

	void GameLayer::OnAttach()
	{
		// Load and setup the sprite sheet image
		m_PieceSpriteSheetTexture = std::make_unique<Texture>("assets/textures/ChessPieces.png");
		m_PieceSpriteSheetTexture->SetWidth(PIECE_SIZE);
		m_PieceSpriteSheetTexture->SetHeight(PIECE_SIZE);

		// Frambuffer init
		Window& window = Application::Get().GetWindow();
		m_Framebuffer = std::make_unique<Framebuffer>(window.GetWidth(), window.GetHeight());

		// Board init
		m_Board = std::make_unique<Board>(glm::vec2(m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight()));

		// Pieces init
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
					if (sdlEvent.key.keysym.sym == SDLK_F4 && sdlEvent.key.repeat == 0 && !s_IsComputerTurn)
					{
						//s_TCPConnection.SendCommand();
						s_IsComputerTurn = true;
						Sleep(150);
					}
					break;
				}

				case SDL_MOUSEMOTION:
				{
					int MouseX{ 0 }, MouseY{ 0 };
					SDL_GetMouseState(&MouseX, &MouseY);
					m_MousePos = { MouseX, MouseY };

					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if (sdlEvent.button.button == SDL_BUTTON_LEFT && !s_IsComputerTurn)
					{
						for (Piece& piece : m_ChessPieces)
						{
							if (IsPointInRect(m_ViewportMousePos, piece.GetBounds()) && !s_IsHoldingPiece)
							{
								m_CurrentPiece = &piece;
								s_IsHoldingPiece = true;
							}
						}
					}
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					if (sdlEvent.button.button == SDL_BUTTON_LEFT && s_IsHoldingPiece)
					{
						UpdatePlayerMove();
						s_IsHoldingPiece = false;
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
			m_Board->OnViewportResize({ m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight() });

			for (auto& piece : m_ChessPieces) piece.OnViewportResize();
		}

		// Camera taking pictures
		if (m_CameraDataReceived)
		{
			LOG_INFO("Data Received: {0}", m_CameraDataBuffer.data());
			m_CameraDataReceived = false;
		}

		// Dragging a piece
		if (s_IsHoldingPiece)
			m_CurrentPiece->SetPosition(m_ViewportMousePos.x, m_ViewportMousePos.y);

		// A new move has been played
		if (m_MoveHistorySize != m_MoveHistory.size())
		{
			m_Board->OnNewMove(m_CurrentMove, m_CurrentPiece);

			m_MoveHistorySize = m_MoveHistory.size();
			s_IsMovePlayed = true;

			// Update current player
			++m_CurrentPlayer;
		}

		m_Board->UpdateActiveSquares();
	}

	void GameLayer::OnRender()
	{
		// Render to ImGui's viewport window
		m_Framebuffer->Bind();

		Renderer::SetClearColor(m_SettingsPanel.GetClearColor());
		Renderer::Clear();

		// Draw all the chess board squares
		m_Board->OnRender();
		
		// Draw all the chess pieces
		for (Piece& piece : m_ChessPieces)
		{
			if (piece.IsCaptured()) continue;	// don't draw captured pieces
			m_PieceSpriteSheetTexture->SetClip(&piece.GetTextureClip());
			m_PieceSpriteSheetTexture->SetPosition(piece.GetPosition().x, piece.GetPosition().y);
			Renderer::DrawTexture(m_PieceSpriteSheetTexture.get());
		}

		// Draw the selected chess piece on top of all other chess pieces
		m_PieceSpriteSheetTexture->SetClip(&m_CurrentPiece->GetTextureClip());
		m_PieceSpriteSheetTexture->SetPosition(m_CurrentPiece->GetPosition().x, m_CurrentPiece->GetPosition().y);
		Renderer::DrawTexture(m_PieceSpriteSheetTexture.get());

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
		const ImGuiViewport* viewport{ ImGui::GetMainViewport() };
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_NoDocking };
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));		
		ImGui::Begin("DockSpace", (bool*)0, windowFlags);
		ImGui::PopStyleVar(3);

		// Set minimum window size
		ImGuiStyle& style{ ImGui::GetStyle() };
		float minWinSizeX{ style.WindowMinSize.x };
		style.WindowMinSize.x = 300.0f;

		// DockSpace
		ImGuiID dockspace_id{ ImGui::GetID("MyDockSpace") };
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		style.WindowMinSize.x = minWinSizeX;

		// Settings Panel
		m_SettingsPanel.OnImGuiRender();

		// Console Panel
		m_ConsolePanel.OnImGuiRender();

		/////////////////////////////////////////////////////////////////////////////////////////////
		//// Viewport Window ////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_ViewportMousePos.x = m_MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
		m_ViewportMousePos.y = m_MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();

		ImVec2 viewportPanelSize{ ImGui::GetContentRegionAvail() };
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		SDL_Texture* textureID{ m_Framebuffer->GetSDLTexture() };
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
		// Grab the selected chess piece's old and new positions
		const std::string oldPos{ m_CurrentMove[0], m_CurrentMove[1] };
		const std::string newPos{ m_CurrentMove[2], m_CurrentMove[3] };

		// Find the corresponding squares inside the squares array
		auto& boardSquares = m_Board->GetBoardSquares();
		auto oldSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return sq.Notation == oldPos; });
		
		auto newSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return sq.Notation == newPos; });
		
		if (oldSquareItr != std::end(boardSquares) && newSquareItr != std::end(boardSquares))
		{
			// Find the corresponding piece
			auto currentPieceItr = std::find_if(std::begin(m_ChessPieces), std::end(m_ChessPieces),
				[&](const Piece& piece) { return piece.GetNotation() == oldSquareItr->Notation; });
			
			if (currentPieceItr != std::end(m_ChessPieces))
			{
				m_CurrentPiece = &(*currentPieceItr);

				// Pawn Promotions
				if (currentPieceItr->IsPromotion(m_CurrentMove))
					currentPieceItr->Promote(m_CurrentMove);

				// Move chess piece
				const glm::vec2 center = newSquareItr->GetCenter();
				currentPieceItr->SetPosition(center.x, center.y);
				currentPieceItr->SetNotation(newSquareItr->Notation);
				currentPieceItr->CheckEnPassant(oldPos);

				m_MoveHistory += m_CurrentMove + ' ';
				LOG_INFO("Computer moved: {0}", m_CurrentMove);
				m_ConsolePanel.AddLog(std::string("Computer moved: " + m_CurrentMove));

				// Remove captured pieces
				CheckPieceCapture();
			}
		}
	}

	void GameLayer::UpdatePlayerMove()
	{
		// Find the square where the piece was dropped at
		auto& boardSquares = m_Board->GetBoardSquares();
		auto targetSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return IsPointInRect(m_ViewportMousePos, sq.GetBounds()); });

		if (targetSquareItr != std::end(boardSquares))
		{
			// If piece was released at same position it was
			if (m_CurrentPiece->GetNotation() == targetSquareItr->Notation)
			{
				const glm::vec2 center = targetSquareItr->GetCenter();
				m_CurrentPiece->SetPosition(center.x, center.y);
				return;
			}

			// Current move is the original piece notation + notation of the square it was dropped at
			m_CurrentMove = { m_CurrentPiece->GetNotation() + targetSquareItr->Notation };

			// Pawn Promotions
			if (m_CurrentPiece->IsPromotion(m_CurrentMove))
			{
				m_CurrentGameState = GameState::PawnPromotion;
				return;
			}

			if (IsCurrentMoveLegal())
			{
				// Move Piece position
				const glm::vec2 center = targetSquareItr->GetCenter();
				m_CurrentPiece->SetPosition(center.x, center.y);
				const std::string oldPos = m_CurrentPiece->GetNotation();
				m_CurrentPiece->SetNotation(targetSquareItr->Notation);
				m_CurrentPiece->CheckEnPassant(oldPos);

				// Capture a piece (if any)
				CheckPieceCapture();

				// Update move history
				m_MoveHistory += m_CurrentMove + ' ';

				std::string msg{ "Player move: " + m_CurrentMove };
				LOG_INFO(msg);
				m_ConsolePanel.AddLog(msg);

				return;
			}
		}
		
		// Reaching here means the move piece was not released at a valid location
		m_ConsolePanel.AddLog(" Wait... that's illegal!\n");

		auto originalSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return m_CurrentPiece->GetNotation() == sq.Notation; });
		
		if (originalSquareItr != std::end(boardSquares))
			m_CurrentPiece->SetPosition(originalSquareItr->GetCenter().x, originalSquareItr->GetCenter().y);
	}

	void GameLayer::ResetPieces()
	{
		// Set piece starting positions and properties
		size_t i{ 0 };
		uint32_t offset{ 0 };
		Piece::Color color{ Piece::Color::Black };
		for (Piece& piece : m_ChessPieces)
		{
			if (i > 15) { offset = 32; color = Piece::Color::White; }

			// Set up the piece properties
			auto& boardSquares = m_Board->GetBoardSquares();
			piece.SetPosition(boardSquares[i + offset].GetCenter().x, boardSquares[i + offset].GetCenter().y);
			piece.SetNotation(boardSquares[i + offset].Notation);
			piece.SetIndex(i);
			piece.SetType();
			piece.SetColor(color);
			piece.SetTextureClip();
			piece.SetEnPassant(false);
			piece.SetCaptured(false);
			++i;
		}
	}

	void GameLayer::CheckPieceCapture()
	{
		for (Piece& piece : m_ChessPieces)
		{
			if (m_CurrentPiece->GetNotation() == piece.GetNotation() &&
				!(m_CurrentPiece->GetIndex() == piece.GetIndex()))	// don't capture self
			{
				piece.Capture();
				break;
			}
		}
	}

	void GameLayer::PromotePawn()
	{
		const std::string newPos = { m_CurrentMove[2], m_CurrentMove[3] };

		auto& boardSquares = m_Board->GetBoardSquares();
		auto targetSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return sq.Notation == newPos; });

		if (targetSquareItr != std::end(boardSquares))
		{
			m_CurrentPiece->Promote(m_CurrentMove);
			const glm::vec2 center = targetSquareItr->GetCenter();
			m_CurrentPiece->SetPosition(center.x, center.y);
			m_CurrentPiece->SetNotation(targetSquareItr->Notation);

			// Capture a piece (if any)
			CheckPieceCapture();

			// Update move history
			m_MoveHistory += m_CurrentMove + ' ';

			// Print message to consoles
			const std::string msg{ "Player move: " + m_CurrentMove };
			LOG_INFO(msg);
			m_ConsolePanel.AddLog(msg);

			//m_IsPlayerPlayed = true;
		}
	}

	void GameLayer::ResetGame()
	{
		m_CurrentPlayer = Player::White;
		m_CurrentGameState = GameState::Gameplay;

		m_CurrentMove.clear();
		m_MoveHistory.clear();
		m_MoveHistorySize = 0;
		m_Board->ResetActiveSquares();

		ResetPieces();
		m_ChessEngine->NewGame();
		m_LegalMoves = m_ChessEngine->GetValidMoves(m_StartPosFEN);
	}

	bool GameLayer::IsPointInRect(const glm::vec2& point, const RectBounds& rect)
	{
		return ((point.x >= rect.left) && (point.x <= rect.right) &&
			(point.y >= rect.bottom) && (point.y <= rect.top)) ? true : false;
	}

	bool GameLayer::IsCurrentMoveLegal()
	{
		// Compare current move to all the legal moves
		auto itr = std::find(std::begin(m_LegalMoves), std::end(m_LegalMoves), m_CurrentMove);
		return (itr != std::end(m_LegalMoves)) ? true : false;
	}

	void GameLayer::GameoverPopup()
	{
		ImGui::SetNextWindowPos({ (m_ViewportSize.x / 2.0f) - 100, m_ViewportSize.y / 2.0f });
		ImGui::SetNextWindowSize({ 200, 80 });
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		ImGui::Begin("Gameover", nullptr, windowFlags);

		const std::string winner = (m_CurrentPlayer == Player::Black) ?
			"White Won!" : "Black Won!";

		auto boldFont = ImGui::GetIO().Fonts->Fonts[2];
		ImGui::PushFont(boldFont);
		ImGui::Text(winner.c_str());
		if (ImGui::Button("Play Again", { 200, 50 }))
			ResetGame();
		ImGui::PopFont();

		ImGui::End();	// End "Gameover"
	}

	void GameLayer::PawnPromotionPopup()
	{
		ImGui::SetNextWindowPos({ (m_ViewportSize.x / 2.0f) - 100, m_ViewportSize.y / 2.0f });
		ImGui::SetNextWindowSize({ 200, 120 });
		ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
		ImGui::Begin("PawnPromo", nullptr, winFlags);

		auto boldFont = ImGui::GetIO().Fonts->Fonts[2];
		ImGui::PushFont(boldFont);
		ImGui::Text("Select piece:");

		bool update{ false };

		if (ImGui::Button("Knight", { 90, 40 }))
		{
			m_CurrentMove.push_back('n');
			update = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Bishop", { 90, 40 }))
		{
			m_CurrentMove.push_back('b');
			update = true;
		}

		if (ImGui::Button("Rook", { 90, 40 }))
		{
			m_CurrentMove.push_back('r');
			update = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Queen", { 90, 40 }))
		{
			m_CurrentMove.push_back('q');
			update = true;
		}

		ImGui::PopFont();

		if (update)
		{
			PromotePawn();
			m_CurrentGameState = GameState::Gameplay();
		}

		ImGui::End();	// End "PawnPromo"
	}

	unsigned int __stdcall GameLayer::ChessEngineThread(void* data)
	{
		GameLayer* Game = static_cast<GameLayer*>(data);

		wchar_t path_Stockfish14_avx2[] = L"assets/engines/stockfish/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe";
		Game->m_ChessEngine->ConnectToEngine(path_Stockfish14_avx2);

		Game->m_LegalMoves = Game->m_ChessEngine->GetValidMoves(Game->m_StartPosFEN);
		Game->m_CurrentFEN = { "\"" + Game->m_ChessEngine->GetFEN(" ") + "\"" };

		while (s_IsEngineThreadRunning)
		{
			if (s_IsMovePlayed && !s_IsComputerTurn)
			{
				// Update legal moves list
				Game->m_CurrentFEN = { "\"" + Game->m_ChessEngine->GetFEN(Game->m_MoveHistory) + "\"" };
				Game->m_LegalMoves = Game->m_ChessEngine->GetValidMoves(Game->m_CurrentFEN);
				
				if (Game->m_LegalMoves.empty())
					Game->m_CurrentGameState = GameState::Gameover;

				// Make computer play automatically after player
				//if (Game->m_IsPlayerPlayed)
				//{
					//game->m_IsComputerTurn = true;
					//Game->m_IsPlayerPlayed = false;
				//}

				s_IsMovePlayed = false;
			}

			if (s_IsComputerTurn)
			{
				LOG_INFO("Computer is playing...");
				Game->m_ConsolePanel.AddLog("Computer is playing...");

				// Get computer's move
				Game->m_CurrentMove = Game->m_ChessEngine->GetNextMove(Game->m_MoveHistory);
				if (Game->m_CurrentMove == "error")
				{
					LOG_ERROR("Failed to get engine move.");
					Game->m_ConsolePanel.AddLog("Failed to get engine move.");
					Game->m_ConsolePanel.AddLog("Enter <F4> to try again.");
				}
				else
				{
					Game->UpdateComputerMove();
					s_IsComputerTurn = false;
				}
			}
		}

		return 100;
	}

	GameLayer::Player operator++(GameLayer::Player& player)
	{
		return player = (player == GameLayer::Player::White) ?
			GameLayer::Player::Black : GameLayer::Player::White;
	}
}

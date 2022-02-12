#include "pch.h"
#include "Chesster_Unleashed/Layers/GameLayer.h"

#include "Chesster_Unleashed/Core/Application.h"
#include "Chesster_Unleashed/Renderer/Renderer.h"
#include "Chesster_Unleashed/Renderer/RenderCommand.h"
#include "Chesster_Unleashed/Renderer/Framebuffer.h"

#include <SDL.h>

namespace Chesster
{
	static bool s_IsThreadRunning{ true };

	ConsolePanel GameLayer::m_ConsolePanel{};
	SettingsPanel GameLayer::m_SettingsPanel{};

	ClientTCP GameLayer::m_ClientTCP{};

	GameLayer::GameLayer() :
		Layer("GameLayer"),
		m_Window{ Application::Get().GetWindow() },
		m_StartPosFEN{ "\"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"" },
		m_PathPythonScript{ "assets/script/__init__.exe" }
	{
		hThread = (HANDLE)_beginthreadex(nullptr, 0, &GameLayer::EngineThread, (void*)&*this, 0, &threadID);
	}

	void GameLayer::OnAttach()
	{
		// Frambuffer init
		FramebufferSpecification framebufferSpec;
		framebufferSpec.Width = m_Window.GetWidth();
		framebufferSpec.Height = m_Window.GetHeight();
		m_Framebuffer = std::make_shared<Framebuffer>(framebufferSpec);
		
		// Board init
		m_Board.Init(glm::vec2(m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height));

		// Pieces init
		Piece::SetPieceClips(m_PieceClips);
		ResetPieces();
	}

	void GameLayer::OnDetach()
	{
		s_IsThreadRunning = false;
		CloseHandle(hThread);
	}

	void GameLayer::OnEvent(SDL_Event& sdlEvent)
	{
		if (m_CurrentGameState == GameState::Gameplay)
		{
			switch (sdlEvent.type)
			{
				case SDL_KEYDOWN:
				{
					if (sdlEvent.key.keysym.sym == SDLK_SPACE && sdlEvent.key.repeat == 0 && !m_IsRecvComputerMove)
					{
						//m_ClientTCP.SendCommand();
						m_IsComputerTurn = true;
					}
					break;
				}

				case SDL_MOUSEMOTION:
				{
					int MouseX{ 0 }, MouseY{ 0 };
					m_MouseButton = SDL_GetMouseState(&MouseX, &MouseY);
					m_MouseCoords = { MouseX, MouseY };

					//CHESSTER_TRACE("mouseCoords({0}, {1})", m_MouseCoords.x, m_MouseCoords.y);
					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if (sdlEvent.button.button == SDL_BUTTON_LEFT && !m_IsComputerTurn && !m_IsRecvComputerMove)
					{
						for (const Piece& piece : m_Pieces)
						{
							if (IsPointInQuad(m_ViewportMousePos, piece.WorldBounds) && !m_IsHoldingPiece)
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

				default:
					break;
			}
		}
	}

	void GameLayer::OnUpdate(const std::chrono::duration<double>& dt)
	{
		// On ImGui window resize
		FramebufferSpecification spec = m_Framebuffer->GetSpec();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Board.OnViewportResize({m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height});

			for (auto& p : m_Pieces) p.OnViewportResize();
		}

		// Chess engine's move
		if (m_IsRecvComputerMove)
			UpdateComputerMove();

		// Dragging a piece
		if (m_IsHoldingPiece)
			m_Pieces[m_PieceIndex].SetPosition(m_ViewportMousePos.x, m_ViewportMousePos.y);

		if (m_IsPieceReleased)
		{
			UpdatePlayerMove();

			// Guard against out of board placement
			if (m_IsOutsideBoard)
			{
				if (IsNotationValid(m_Pieces[m_PieceIndex].Notation))
				{
					m_Pieces[m_PieceIndex].SetPosition(m_TargetSquare.Center.x, m_TargetSquare.Center.y);
					m_ConsolePanel.AddLog("Hey! The board is over there!\n\n");
					CHESSTER_ERROR("Hey! The board is over there!");
				}
			}
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

		if (SettingsPanel::IsCameraButton)
		{
			if (SettingsPanel::IsCameraConnected)
			{
				m_ClientTCP.DisconnectCamera();
				SettingsPanel::IsCameraConnected = false;
			}
			else
			{
				SettingsPanel::IsCameraConnected = true;
				m_ClientTCP.ConnectCamera();
				m_ClientTCP.SendCameraCommand("SE8");
				if (!m_ClientTCP.RecvCameraConfirmation())
				{
					m_ConsolePanel.AddLog("Camera did not connect sucessfully.");
					CHESSTER_WARN("Camera did not connect sucessfully.");
					SettingsPanel::IsCameraConnected = false;
				}
			}

			SettingsPanel::IsCameraButton = false;
		}

		if (SettingsPanel::IsRobotButton)
		{
			if (SettingsPanel::IsRobotConnected)
			{
				m_ClientTCP.DisconnectRobot();
				SettingsPanel::IsRobotConnected = false;
			}
			else
			{
				SettingsPanel::IsRobotConnected = true;
				if (!m_ClientTCP.ConnectRobot())
				{
					m_ConsolePanel.AddLog("Program did not connect to Staubli Robot sucessfully.");
					CHESSTER_WARN("Program did not connect to Staubli Robot sucessfully.");
					SettingsPanel::IsRobotConnected = false;
				}

				//m_ClientTCP.SendCommand("");
			}

			SettingsPanel::IsRobotButton = false;
		}

		// Check for New Game (take pic and compare, etc)
		m_NewGameData = m_ClientTCP.GetCameraData();

		UpdateDifficulty();

		m_Board.OnUpdate(dt);
	}

	void GameLayer::OnRender()
	{
		m_Framebuffer->Bind();

		RenderCommand::SetClearColor(SettingsPanel::ClearColor);
		RenderCommand::Clear();

		m_Board.OnRender();
		
		for (auto& piece : m_Pieces)
			piece.OnRender();

		// Draw selected piece on top of all others
		Renderer::DrawTextureEx(&m_Pieces[m_PieceIndex].Texture);

		if (m_CurrentGameState == GameState::Gameover)
		{
			SDL_Rect blackOverlay = { 0, 0, m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height };
			glm::vec4 blackOverlayColor = { 0, 0, 0, 150 };
			Renderer::DrawFilledRect(blackOverlay, blackOverlayColor);
		}

		m_Framebuffer->Unbind();
	}

	void GameLayer::OnImGuiRender()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));		
		ImGui::Begin("DockSpace", (bool*)0, window_flags);
		ImGui::PopStyleVar(3);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		static bool opt_fullscreen{ false };
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen))
				{
					SDL_Event e{};
					e.type = SDL_KEYDOWN;
					e.key.keysym.sym = SDLK_F5;
					SDL_PushEvent(&e);
				}

				if (ImGui::MenuItem("Exit")) Application::Get().Quit();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Settings Panel
		m_SettingsPanel.OnImGuiRender();

		// Console Panel
		m_ConsolePanel.OnImGuiRender("Chess Engine");

		/////////////////////////////////////////////////////////////////////////////////////////////
		//// Viewport Window ////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		m_ViewportMousePos.x = m_MouseCoords.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
		m_ViewportMousePos.y = m_MouseCoords.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();
		//CHESSTER_TRACE("{0}, {1}", m_ViewportMousePos.x, m_ViewportMousePos.y);

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
		m_ConsolePanel.AddLog("Computer is playing...");
		CHESSTER_INFO("Computer is playing...");

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
				m_Pieces[m_PieceIndex].UpdateTextureClip(m_CurrentMove, m_PieceClips);
			
			m_Pieces[m_PieceIndex].SetPosition(m_TargetSquare.Center.x, m_TargetSquare.Center.y);
			m_Pieces[m_PieceIndex].Notation = m_TargetSquare.Notation;
			m_Pieces[m_PieceIndex].CheckEnPassant(oldPos);
			m_MoveHistory += m_CurrentMove + ' ';
			m_ConsolePanel.AddLog(std::string("Computer moved: " + m_CurrentMove).c_str());

			// Remove captured pieces
			for (Piece& piece : m_Pieces)
				if (m_Pieces[m_PieceIndex].Notation == piece.Notation && !(m_Pieces[m_PieceIndex].Index == piece.Index))
					RemovePiece(piece);
		}

		m_IsRecvComputerMove = false;
	}

	void GameLayer::UpdatePlayerMove()
	{
		m_IsOutsideBoard = true;
		m_IsPieceReleased = false;

		// Find the square where the piece was dropped at
		for (const Board::Square& square : m_Board.GetBoardSquares())
		{
			if (IsPointInQuad(m_ViewportMousePos, square.WorldBounds))
			{
				// If piece was not moved to a new square
				if (m_Pieces[m_PieceIndex].Notation == square.Notation)
				{
					m_Pieces[m_PieceIndex].SetPosition(square.Center.x, square.Center.y);
					m_IsOutsideBoard = false;
					break;
				}

				m_CurrentMove = { m_Pieces[m_PieceIndex].Notation + square.Notation };

				// Pawn Promotions
				if (m_Pieces[m_PieceIndex].IsPromotion(m_CurrentMove))
				{
					m_CurrentGameState = GameState::PawnPromotion;
					m_IsOutsideBoard = false;
				}

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
						m_ConsolePanel.AddLog("Player move: %s", m_CurrentMove);
						CHESSTER_INFO(std::string("Player move: " + m_CurrentMove).c_str());


						m_IsPlayerPlayed = true;
						m_IsOutsideBoard = false;
						break;
					}
					else
					{
						// Illegal move, return piece to original position
						if (IsNotationValid(m_Pieces[m_PieceIndex].Notation))
						{
							m_Pieces[m_PieceIndex].SetPosition(m_TargetSquare.Center.x, m_TargetSquare.Center.y);
							m_ConsolePanel.AddLog(" Wait... that's illegal!\n");
							CHESSTER_ERROR("Wait... that's illegal!");
							m_IsOutsideBoard = false;
							break;
						}
					}
				}
			}
		}
	}

	void GameLayer::RemovePiece(Piece& piece)
	{
		std::string wasCapturedAt = piece.Notation;

		piece.Notation = "00";
		piece.SetPosition(-3000.0f, -3000.0f);
		piece.IsCaptured = true;
	}

	void GameLayer::ResetPieces()
	{
		Application& app = Application::Get();

		// Set piece starting positions and properties
		size_t i{ 0 };
		uint32_t offset{ 0 };
		PieceColor color = PieceColor::Black;
		for (Piece& piece : m_Pieces)
		{
			if (i > 15) { offset = 32; color = PieceColor::White; }
			auto& squares = m_Board.GetBoardSquares();

			// Set up the sprite
			piece.Texture = app.Get().m_TextureHolder.Get(TextureID::Pieces);
			piece.Texture.SetWidth(piece.Size.x);
			piece.Texture.SetHeight(piece.Size.y);
			piece.Texture.SetClip(&m_PieceClips[i]);

			// Set up the properties
			piece.SetPosition(squares[i + offset].Center.x, squares[i + offset].Center.y);
			piece.Notation = squares[i + offset].Notation;
			piece.Index = i;
			piece.SetType();
			piece.Color = color;
			++i;
		}
	}

	void GameLayer::PromotePawn()
	{
		std::string move = { m_CurrentMove[2], m_CurrentMove[3] };
		auto& x = m_Board.m_SquaresMap.find(move);
		if (x != m_Board.m_SquaresMap.end())
		{
			m_Pieces[m_PieceIndex].UpdateTextureClip(m_CurrentMove, m_PieceClips);
			m_Pieces[m_PieceIndex].SetPosition(x->second->Center.x, x->second->Center.y);
			m_Pieces[m_PieceIndex].Notation = x->second->Notation;
			CHESSTER_INFO("type {0}", m_Pieces[m_PieceIndex].Type);
			// Capture a piece (if any)
			for (Piece& piece : m_Pieces)
				if (m_Pieces[m_PieceIndex].Notation == piece.Notation &&
					!(m_Pieces[m_PieceIndex].Index == piece.Index))
					RemovePiece(piece);

			// Update move history
			m_MoveHistory += m_CurrentMove + ' ';
			m_ConsolePanel.AddLog("Player move: %s", m_CurrentMove);
			CHESSTER_INFO(std::string("Player move: " + m_CurrentMove).c_str());

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

	void GameLayer::UpdateDifficulty()
	{
		if (SettingsPanel::IsNewSkillLevel)
		{
			m_Connector.SetDifficultyLevel(SettingsPanel::SkillLevel);
			SettingsPanel::IsNewSkillLevel = false;
		}

		if (SettingsPanel::IsNewELO)
		{
			m_Connector.SetDifficultyELO(SettingsPanel::ELO);
			SettingsPanel::IsNewELO = false;
		}

		if (SettingsPanel::IsToggleELO)
		{
			m_Connector.ToggleELO(SettingsPanel::IsELOActive);
			SettingsPanel::IsToggleELO = false;
		}
	}

	bool GameLayer::IsPointInQuad(const glm::vec2& point, const QuadBounds& quad)
	{
		return ((point.x >= quad.left) && (point.x <= quad.right) &&
			(point.y >= quad.bottom) && (point.y <= quad.top)) ? true : false;
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
		GameLayer* game = static_cast<GameLayer*>(data);

		//wchar_t path_Stockfish5[] = L"assets/engines/stockfish/stockfish_5.exe";
		//wchar_t path_Stockfish14_popcnt[] = L"assets/engines/stockfish/stockfish_14.1_win_x64_popcnt/stockfish_14.1_win_x64_popcnt.exe";
		wchar_t path_Stockfish14_avx2[] = L"assets/engines/stockfish/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe";

		game->m_Connector.ConnectToEngine(path_Stockfish14_avx2);

		game->m_LegalMoves = game->m_Connector.GetValidMoves(game->m_PathPythonScript, game->m_StartPosFEN);
		game->m_CurrentFEN = { "\"" + game->m_Connector.GetFEN(" ") + "\"" };

		while (s_IsThreadRunning)
		{
			if (game->m_IsMovePlayed && !game->m_IsComputerTurn)
			{
				// Update legal moves list
				game->m_CurrentFEN = { "\"" + game->m_Connector.GetFEN(game->m_MoveHistory) + "\"" };
				game->m_LegalMoves = game->m_Connector.GetValidMoves(game->m_PathPythonScript, game->m_CurrentFEN);
				
				if (game->m_LegalMoves.empty())
					game->m_CurrentGameState = GameState::Gameover;

				// Make computer play automatically after player
				if (game->m_IsPlayerPlayed)
				{
					//game->m_IsComputerTurn = true;
					game->m_IsPlayerPlayed = false;
				}

				game->m_IsMovePlayed = false;
			}

			if (game->m_IsComputerTurn)
			{
				// Get computer's move
				game->m_CurrentMove = game->m_Connector.GetNextMove(game->m_MoveHistory);
				if (game->m_CurrentMove == "error")
				{
					game->m_ConsolePanel.AddLog("Failed to get engine move.");
					game->m_ConsolePanel.AddLog("Enter <spacebar> to try again.");
					CHESSTER_ERROR("Failed to get engine move.");
					game->m_IsRecvComputerMove = false;
				}
				else
				{
					game->m_IsRecvComputerMove = true;
					game->m_IsComputerTurn = false;
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

#include "pch.h"
#include "Chesster/Layers/GameLayer.h"

#include <imgui.h>

namespace Chesster
{
	std::atomic<bool> a_IsChessEngineRunning{ false };	// ChessEngineThread loops while this is true
	std::atomic<bool> a_IsMovePlayed{ false };			// Updates m_LegalMoves when a new move has been played
	std::atomic<bool> a_IsComputerTurn{ false };		// Allows the chess engine to play

	static bool s_IsHoldingPiece{ false };				// Moves the Piece's sprite while user is holding it with mouse

	const std::string START_FEN = { "\"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"" };
	
	GameLayer* GameLayer::s_Instance{ nullptr };

	GameLayer::GameLayer() :
		Layer("GameLayer"),
		m_Framebuffer{ 480, 360 },
		m_ViewportSize{ 0.0f, 0.0f },
		m_MousePos{ 0.0f, 0.0f },
		m_ViewportMousePos{ 0.0f, 0.0f },
		m_OldCameraData{ "a1R", "a2P", "a7p", "a8r", "b1N", "b2P", "b7p", "b8n", "c1B",
		"c2P", "c7p", "c8b", "d1Q", "d2P", "d7p", "d8q", "e1K", "e2P", "e7p", "e8k", "f1B",
		"f2P", "f7p", "f8b", "g1N", "g2P", "g7p", "g8n", "h1R", "h2P", "h7p", "h8r" }
	{
		s_Instance = this;
	}

	void GameLayer::OnAttach()
	{
		// Connect Stockfish
		a_IsChessEngineRunning = true;
		std::thread engineThread(ChessEngineThread);
		engineThread.detach();
	}

	void GameLayer::OnDetach()
	{
		a_IsChessEngineRunning = false;
	}

	void GameLayer::OnEvent(SDL_Event& sdlEvent)
	{
		if (m_CurrentGameState == GameState::Gameplay)
		{
			switch (sdlEvent.type)
			{
				case SDL_KEYDOWN:
				{
					if (sdlEvent.key.keysym.sym == SDLK_F4 && sdlEvent.key.repeat == 0 && !a_IsComputerTurn)
					{
						a_IsComputerTurn = true;
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
					if (sdlEvent.button.button == SDL_BUTTON_LEFT && !a_IsComputerTurn)
					{
						for (Piece& piece : m_Board.GetChessPieces())
						{
							if (IsPointInRect(m_ViewportMousePos, piece.GetBounds()) && !s_IsHoldingPiece)
							{
								m_Board.SetCurrentPiece(&piece);
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
						UpdatePlayerMouseMove();
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
			(m_Framebuffer.GetWidth() != m_ViewportSize.x || m_Framebuffer.GetHeight() != m_ViewportSize.y))
		{
			m_Framebuffer.Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Board.OnViewportResize({ m_Framebuffer.GetWidth(), m_Framebuffer.GetHeight() });
		}

		// Camera taking pictures
		if (m_CameraDataReceived)
		{
			std::vector<std::string> tempCameraData;
			std::istringstream iss{ m_CameraDataBuffer.data() };
			for (std::string piece; iss >> piece;)
			{
				if (piece.front() != '0')
					tempCameraData.push_back(piece);
			}

			std::sort(tempCameraData.begin(), tempCameraData.end());

			if (m_NewCameraData != tempCameraData)
			{
				m_NewCameraData = tempCameraData;
				UpdatePlayerCameraMove();
			}

			m_CameraDataReceived = false;
		}

		// Dragging a piece with mouse
		if (s_IsHoldingPiece)
			m_Board.GetCurrentPiece().SetPosition(m_ViewportMousePos);

		// A new move has been played
		if (m_MoveHistorySize != m_MoveHistory.size())
		{
			m_Board.UpdateNewMove(m_CurrentMove);

			m_MoveHistorySize = m_MoveHistory.size();
			a_IsMovePlayed = true;

			// Update current player
			++m_CurrentPlayer;

			m_Board.UpdateActiveSquares(m_CurrentMove);
		}
	}

	void GameLayer::OnRender()
	{
		// Render to ImGui's viewport window
		m_Framebuffer.Bind();

		Renderer::SetClearColor(m_SettingsPanel.GetClearColor());
		Renderer::Clear();

		// Draw all the chess board squares and chess pieces
		m_Board.OnRender();

		// Draw a faded black screen when gameover
		if (m_CurrentGameState == GameState::Gameover)
		{
			const SDL_Rect blackOverlayRect = { 0, 0, m_Framebuffer.GetWidth(), m_Framebuffer.GetHeight() };
			const glm::vec4 blackOverlayColor = { 0, 0, 0, 150 };
			Renderer::DrawFilledRect(blackOverlayRect, blackOverlayColor);
		}

		m_Framebuffer.Unbind();
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
		SDL_Texture* textureID{ m_Framebuffer.GetSDLTexture() };
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
		m_Board.MovePiece(m_CurrentMove);

		// Pawn Promotions
		Piece& currentPiece = m_Board.GetCurrentPiece();
		if (currentPiece.IsPromotion(m_CurrentMove))
			currentPiece.Promote(m_CurrentMove);

		// Remove captured pieces
		m_Board.UpdatePieceCapture();

		m_MoveHistory += m_CurrentMove + ' ';
		LOG_INFO("Computer moved: {0}", m_CurrentMove);
		m_ConsolePanel.AddLog(std::string("Computer moved: " + m_CurrentMove));
	}

	void GameLayer::UpdatePlayerCameraMove()
	{
		m_CurrentMove = GetCameraMove();

		if (IsMoveLegal(m_CurrentMove))
		{
			m_Board.MovePiece(m_CurrentMove);

			// Pawn Promotions
			Piece& currentPiece = m_Board.GetCurrentPiece();
			if (currentPiece.IsPromotion(m_CurrentMove))
				currentPiece.Promote(m_CurrentMove);

			// Capture a piece (if any)
			m_Board.UpdatePieceCapture();

			// Update move history
			m_MoveHistory += m_CurrentMove + ' ';

			std::string msg{ "Player moved: " + m_CurrentMove };
			LOG_INFO(msg);
			m_ConsolePanel.AddLog("\n" + msg);

			m_OldCameraData = m_NewCameraData;
			return;
		}

		// The piece was not placed at a valid location
		m_ConsolePanel.AddLog("Wait... that's illegal!\n");
	}

	void GameLayer::UpdatePlayerMouseMove()
	{
		Piece& currentPiece = m_Board.GetCurrentPiece();

		// Find the square where the piece was dropped at
		auto& boardSquares = m_Board.GetBoardSquares();
		auto targetSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return IsPointInRect(m_ViewportMousePos, sq.GetBounds()); });

		if (targetSquareItr != std::end(boardSquares))
		{
			// If piece was released at same position it was
			if (currentPiece.GetNotation() == targetSquareItr->Notation)
			{
				currentPiece.SetPosition(targetSquareItr->GetCenter());
				return;
			}

			// Current move is the original piece notation + notation of the square it was dropped at
			m_CurrentMove = { currentPiece.GetNotation() + targetSquareItr->Notation };

			// Pawn Promotions
			if (currentPiece.IsPromotion(m_CurrentMove)
				&& (int)currentPiece.GetColor() == (int)m_CurrentPlayer + 1)
			{
				m_CurrentGameState = GameState::PawnPromotion;
				return;
			}

			if (IsMoveLegal(m_CurrentMove))
			{
				m_Board.MovePiece(m_CurrentMove);

				// Capture a piece (if any)
				m_Board.UpdatePieceCapture();

				// Update move history
				m_MoveHistory += m_CurrentMove + ' ';

				std::string msg{ "Player moved: " + m_CurrentMove };
				LOG_INFO(msg);
				m_ConsolePanel.AddLog("\n" + msg);

				return;
			}
		}
		
		// The piece was not released at a valid location
		m_ConsolePanel.AddLog("Wait... that's illegal!\n");

		auto originalSquareItr = std::find_if(std::begin(boardSquares), std::end(boardSquares),
			[&](const Board::Square& sq) { return currentPiece.GetNotation() == sq.Notation; });
		
		// Move it back to its original square
		if (originalSquareItr != std::end(boardSquares))
			currentPiece.SetPosition(originalSquareItr->GetCenter());
	}

	void GameLayer::UpdatePlayerPawnPromotion()
	{
		m_Board.GetCurrentPiece().Promote(m_CurrentMove);
		m_Board.MovePiece(m_CurrentMove);

		// Capture a piece (if any)
		m_Board.UpdatePieceCapture();

		// Update move history
		m_MoveHistory += m_CurrentMove + ' ';
		const std::string msg{ "Player moved: " + m_CurrentMove };
		LOG_INFO(msg);
		m_ConsolePanel.AddLog("\n" + msg);
	}

	std::string GameLayer::GetCameraMove()
	{
		/** Retrieves the difference between two std::vector. 
			This lambda basically does this: difference = dataA - dataB */
		auto getDifference = [](std::vector<std::string>& dataA, std::vector<std::string>& dataB) -> std::vector<std::string>
		{
			std::vector<std::string> difference;
			std::set_difference(dataA.begin(), dataA.end(), dataB.begin(), dataB.end(),
				std::inserter(difference, difference.begin()));

			return difference;
		};

		std::vector<std::string> differenceOld = getDifference(m_OldCameraData, m_NewCameraData);
		std::vector<std::string> differenceNew = getDifference(m_NewCameraData, m_OldCameraData);
		
		/* This lambda erases any std::string from dataA that exists in both dataA and dataB. */
		auto eraseDuplicate = [&](std::vector<std::string>& dataA, std::vector<std::string>& dataB)
		{
			auto itr = std::find_if(std::begin(dataA), std::end(dataA),
			[&](std::string old)
			{
				old.pop_back();
				for (std::string str : dataB)
				{
					str.pop_back();
					if (old == str)
						return true;
				}
				return false;
			});

			if (itr != std::end(dataA))
				dataA.erase(itr);
		};

		// Deal with captures. Captured pieces share the same square notation as its capturer.
		if (differenceOld.size() > differenceNew.size())
			eraseDuplicate(differenceOld, differenceNew);

		if (differenceNew.size() > differenceOld.size())
			eraseDuplicate(differenceNew, differenceOld);

		// Deal with castling. If at this point the std::vectors have more than one
		// notation, it could mean that two pieces were moved and don't share square
		// notation (no capture). In example, it could have been a castling move.
		if (differenceOld.size() > 1 || differenceNew.size() > 1)
		{
			differenceOld.erase(std::remove_if(differenceOld.begin(), differenceOld.end(),
			[](const std::string& oldNotation)
			{
				return oldNotation != "e1K"
					&& oldNotation != "e8k";
			}), differenceOld.end());

			differenceNew.erase(std::remove_if(differenceNew.begin(), differenceNew.end(),
			[](const std::string& newNotation)
			{
				return newNotation != "g1K"
					&& newNotation != "g8k"
					&& newNotation != "c1K"
					&& newNotation != "c8k";
			}), differenceNew.end());
		}

		// If there's only one notation in each std::vector, that's our move
		if (differenceOld.size() == 1 && differenceNew.size() == 1)
		{
			differenceOld.front().pop_back();
			differenceNew.front().pop_back();

			return { differenceOld.front() + differenceNew.front() };
		}

		return "error";
	}

	void GameLayer::ResetGame()
	{
		m_CurrentPlayer = Player::White;
		m_CurrentGameState = GameState::Gameplay;

		m_CurrentMove.clear();
		m_MoveHistory.clear();
		m_MoveHistorySize = 0;
		m_Board.ResetActiveSquares();

		m_Board.ResetPieces();
		m_ChessEngine.NewGame();
		m_LegalMoves = m_ChessEngine.GetValidMoves(START_FEN);
	}

	bool GameLayer::IsPointInRect(const glm::vec2& point, const RectBounds& rect)
	{
		return ((point.x >= rect.left) && (point.x <= rect.right) &&
			(point.y >= rect.bottom) && (point.y <= rect.top)) ? true : false;
	}

	bool GameLayer::IsMoveLegal(const std::string& notation)
	{
		// Compare current move to all the legal moves
		auto itr = std::find(std::begin(m_LegalMoves), std::end(m_LegalMoves), notation);
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

		if (ImGui::Button("Knight", { 96, 45 }))
		{
			m_CurrentMove.push_back('n');
			update = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Bishop", { 96, 45 }))
		{
			m_CurrentMove.push_back('b');
			update = true;
		}

		if (ImGui::Button("Rook", { 96, 45 }))
		{
			m_CurrentMove.push_back('r');
			update = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Queen", { 96, 45 }))
		{
			m_CurrentMove.push_back('q');
			update = true;
		}

		ImGui::PopFont();

		if (update)
		{
			UpdatePlayerPawnPromotion();
			m_CurrentGameState = GameState::Gameplay();
		}

		ImGui::End();	// End "PawnPromo"
	}

	void GameLayer::ChessEngineThread()
	{
		GameLayer& gameLayer = GameLayer::Get();

		wchar_t path_Stockfish14_avx2[] = L"assets/engines/stockfish/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe";
		gameLayer.m_ChessEngine.ConnectToEngine(path_Stockfish14_avx2);
		gameLayer.m_LegalMoves = gameLayer.m_ChessEngine.GetValidMoves(START_FEN);

		while (a_IsChessEngineRunning)
		{
			if (a_IsMovePlayed && !a_IsComputerTurn)
			{
				// Update legal moves list
				const std::string currentFEN = { "\"" + gameLayer.m_ChessEngine.GetFEN(gameLayer.m_MoveHistory) + "\"" };
				gameLayer.m_LegalMoves = gameLayer.m_ChessEngine.GetValidMoves(currentFEN);
					
				if (gameLayer.m_LegalMoves.empty())
					gameLayer.m_CurrentGameState = GameState::Gameover;

				a_IsMovePlayed = false;
			}

			if (a_IsComputerTurn)
			{
				LOG_INFO("Computer is playing...");
				gameLayer.m_ConsolePanel.AddLog("\nComputer is playing...");

				// Get computer's move
				gameLayer.m_CurrentMove = gameLayer.m_ChessEngine.GetNextMove(gameLayer.m_MoveHistory);
				if (gameLayer.m_CurrentMove == "error")
				{
					LOG_ERROR("Failed to get engine move.");
					gameLayer.m_ConsolePanel.AddLog("Failed to get engine move.");
					gameLayer.m_ConsolePanel.AddLog("Enter <F4> to try again.");
				}
				else
				{
					gameLayer.UpdateComputerMove();
					//gameLayer.m_Network.SendToRobot(gameLayer.m_CurrentMove);
				}

				a_IsComputerTurn = false;
			}
		}
	}

	GameLayer::Player operator++(GameLayer::Player& player)
	{
		return player = (player == GameLayer::Player::White) ?
			GameLayer::Player::Black : GameLayer::Player::White;
	}
}

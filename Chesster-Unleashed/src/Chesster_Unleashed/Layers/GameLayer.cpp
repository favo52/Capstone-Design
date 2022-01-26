#include "pch.h"
#include "Chesster_Unleashed/Layers/GameLayer.h"

#include "Chesster_Unleashed/Core/Application.h"
#include "Chesster_Unleashed/Renderer/Renderer.h"
#include "Chesster_Unleashed/Renderer/RenderCommand.h"
#include "Chesster_Unleashed/Renderer/Framebuffer.h"

#include "Chesster_Unleashed/ImGui/Styles.h"

//#include <imgui.h>
#include <SDL.h>

namespace Chesster
{
	ConsolePanel GameLayer::m_ConsolePanel{};

	GameLayer::GameLayer() :
		Layer(""),
		m_Window{ Application::Get().GetWindow() }
	{
	}

	void GameLayer::OnAttach()
	{
		// Frambuffer init
		FramebufferSpecification framebufferSpec;
		framebufferSpec.Width = m_Window.GetWidth();
		framebufferSpec.Height = m_Window.GetHeight();
		m_Framebuffer = std::make_shared<Framebuffer>(framebufferSpec);
		
		m_Board.Init(glm::vec2(m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height));
	}

	void GameLayer::OnDetach()
	{
	}

	void GameLayer::OnEvent(SDL_Event& sdlEvent)
	{
		switch (sdlEvent.type)
		{
			case SDL_KEYDOWN:
			{
				//if (sdlEvent.key.keysym.sym == SDLK_SPACE && sdlEvent.key.repeat == 0 && !m_IsComputerAnimation && !m_IsRecvComputerMove)
				//	m_IsComputerTurn = true;
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
				//if (sdlEvent.button.button == SDL_BUTTON_LEFT)
				//{
				//	for (const Piece& piece : m_Pieces)
				//	{
				//		if (IsPointInQuad(m_MouseCoords, piece.GetWorldBounds()) && !m_IsHoldingPiece)
				//		{
				//			m_PieceIndex = piece.GetPieceData()->Index;
				//			m_IsHoldingPiece = true;
				//		}
				//	}
				//}
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				//if (sdlEvent.button.button == SDL_BUTTON_LEFT && m_IsHoldingPiece)
				//{
				//	m_IsPieceReleased = true;
				//	m_IsHoldingPiece = false;
				//}
				break;
			}

			default:
				break;
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
			m_Board.OnViewportResize({ m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height });
		}

		for (const Board::BoardSquare& square : m_Board.GetBoardSquares())
		{
			if (IsPointInQuad(m_ViewportMousePos, square.WorldBounds))
				CHESSTER_INFO("{0}", square.Notation);
		}
	}

	void GameLayer::OnRender()
	{
		m_Framebuffer->Bind();

		RenderCommand::SetClearColor(ClearColor);
		RenderCommand::Clear();

		// Draw here
		m_Board.OnRender();

		/*SDL_Rect rect = { (m_Framebuffer->GetSpec().Width / 2.0f) - 50.0f, (m_Framebuffer->GetSpec().Height / 2.0f) - 50.0f, 100, 100 };
		glm::vec4 color = { 200, 200, 200, 255 };
		Renderer::DrawFilledRect(rect, color);*/

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

		/////////////////////////////////////////////////////////////////////////////////////////////
		//// Settings Panel /////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		ImGui::Begin("Settings");

		static glm::vec4 clearColor = { 0.141f, 0.203f, 0.270f, 1.0f };
		DrawSection<glm::vec4>("Colors", [&]()
		{
			if (DrawColorEdit4Control("Border", clearColor, 60.0f))
				ClearColor = clearColor * 255.0f;

			if (DrawColorEdit4Control("Evens", SquareColor1, 60.0f))
				UpdateSquareColors();

			if (DrawColorEdit4Control("Odds", SquareColor2, 60.0f))
				UpdateSquareColors();
		});
		
		ImGui::Separator();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::End(); // End "Stats"

		/////////////////////////////////////////////////////////////////////////////////////////////
		//// Console Panel //////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		m_ConsolePanel.OnImGuiRender("Console Panel");
		
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
		ImGui::Image((ImTextureID*)(intptr_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		ImGui::End(); // End "Viewport"
		ImGui::PopStyleVar();

		ImGui::End(); // End "DockSpace"
	}

	void GameLayer::UpdateSquareColors()
	{
		for (Board::BoardSquare& square : m_Board.GetBoardSquares())
		{
			glm::vec4 newColor = ((square.Index + 1) % 2 == 0) ? SquareColor2 : SquareColor1;
			square.Color = newColor * 255.0f;
		}
	}

	bool GameLayer::IsPointInQuad(const glm::vec2& point, const QuadBounds& quad)
	{
		return ((point.x >= quad.left) && (point.x <= quad.right) &&
			(point.y >= quad.bottom) && (point.y <= quad.top)) ? true : false;
	}
}

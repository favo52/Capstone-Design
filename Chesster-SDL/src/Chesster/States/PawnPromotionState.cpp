#include "pch.h"
#include "PawnPromotionState.h"

#include "imgui.h"

namespace Chesster
{
	char PawnPromotionState::m_Piece = 'x';
	constexpr int m_PieceSize{ 80 };

	PawnPromotionState::PawnPromotionState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_MousePos{},
		m_TouchLocation{},
		m_Background{ (static_cast<int>((m_Window->GetWidth()) - 160) / 2) - 280, (static_cast<int>(m_Window->GetHeight()) - 160) / 2, 80 * 2, 80 * 2 },
		m_Figures{ new Texture[4] },
		m_CurrentOption{ Piece::Queen }
	{
		m_Font = context.fonts->Get(FontID::Minecraft);
		m_TextTexture.LoadFromRenderedText(m_Font, "CHOOSE PROMOTION:", { 21u, 21u, 21u, 255u });
		m_TextTexture.SetPosition(((m_Window->GetWidth() - m_TextTexture.GetWidth()) / 2) - 280, ((m_Window->GetHeight() - m_TextTexture.GetHeight()) / 2) - 100);

		for (int i = 0; i < 4; ++i)
			m_Figures[i] = context.textures->Get(TextureID::Pieces);

		m_PieceClip[Piece::Knight] = { m_PieceSize * 1, m_PieceSize * 1, m_PieceSize, m_PieceSize };
		m_PieceClip[Piece::Bishop] = { m_PieceSize * 2, m_PieceSize * 1, m_PieceSize, m_PieceSize };
		m_PieceClip[Piece::Rook] = { m_PieceSize * 0, m_PieceSize * 1, m_PieceSize, m_PieceSize };
		m_PieceClip[Piece::Queen] = { m_PieceSize * 3, m_PieceSize * 1, m_PieceSize, m_PieceSize };

		m_Figures[Piece::Knight].SetPosition(m_Background.x, m_Background.y, &m_PieceClip[Piece::Knight]);
		m_Figures[Piece::Bishop].SetPosition(m_Background.x + 80, m_Background.y, &m_PieceClip[Piece::Bishop]);
		m_Figures[Piece::Rook].SetPosition(m_Background.x, m_Background.y + 80, &m_PieceClip[Piece::Rook]);
		m_Figures[Piece::Queen].SetPosition(m_Background.x + 80, m_Background.y + 80, &m_PieceClip[Piece::Queen]);

		m_SelectionHighlight = { m_Figures[Piece::Queen].GetPosition().x, m_Figures[Piece::Queen].GetPosition().y, m_PieceSize, m_PieceSize };
	}

	PawnPromotionState::~PawnPromotionState()
	{
	}

	bool PawnPromotionState::HandleEvent(SDL_Event& event)
	{
		switch (event.type)
		{
		/*case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{

			case SDLK_RETURN:
				break;

			case SDLK_UP:
			{

			} break;

			case SDLK_DOWN:
			{

			} break;
			}
		} break;*/

			case SDL_FINGERMOTION:
			case SDL_MOUSEMOTION:
			{
				// Get the mouse screen coordinates
				SDL_GetMouseState(&m_MousePos.x, &m_MousePos.y);

				// Get finger touch location in screen coordinates
				m_TouchLocation.x = event.tfinger.x * m_Window->GetWidth();
				m_TouchLocation.y = event.tfinger.y * m_Window->GetHeight();

				// Check if mouse is inside texture bounds
				for (int i = 0; i < Piece::Size; ++i)
				{
					SDL_Rect textBounds = m_Figures[i].GetBounds();
					if (SDL_PointInRect(&m_MousePos, &textBounds) ||
						SDL_PointInRect(&m_TouchLocation, &textBounds))
					{
						m_SelectionHighlight = { m_Figures[i].GetPosition().x, m_Figures[i].GetPosition().y, m_PieceSize, m_PieceSize };
						m_CurrentOption = Piece(i);
					}
				}
			} break;

		//case SDL_FINGERUP:
		//{
		//	// Get finger touch location in screen coordinates
		//	m_TouchLocation.x = event.tfinger.x * m_Window->GetWidth();
		//	m_TouchLocation.y = event.tfinger.y * m_Window->GetHeight();

		//	for (int i = 0; i < m_MenuOptions.size(); ++i)
		//	{
		//		SDL_Rect textBounds = m_MenuOptions[i]->GetBounds();
		//		if (SDL_PointInRect(&m_TouchLocation, &textBounds))
		//			SelectOption();
		//	}
		//} break;

			case SDL_MOUSEBUTTONUP:
			{
				if (event.button.button == SDL_BUTTON_LEFT)
					for (int i = 0; i < Piece::Size; ++i)
					{
						SDL_Rect textBounds = m_Figures[i].GetBounds();
						if (SDL_PointInRect(&m_MousePos, &textBounds))
						{
							SelectOption();
							RequestStackPop();
						}
					}
			} break;
		}

		return true;
	}

	bool PawnPromotionState::Update(const std::chrono::duration<double>& dt)
	{
		return false;
	}

	void PawnPromotionState::Draw()
	{
		m_TextTexture.Draw();

		SDL_SetRenderDrawColor(Window::Renderer, 0u, 0u, 0u, 230u);
		SDL_RenderFillRect(Window::Renderer, &m_Background);

		for (int i = 0; i < 4; ++i)
			m_Figures[i].Draw();

		SDL_SetRenderDrawColor(Window::Renderer, 255u, 255u, 0u, 180u);
		SDL_RenderFillRect(Window::Renderer, &m_SelectionHighlight);
	}

	void PawnPromotionState::SelectOption()
	{
		switch (m_CurrentOption)
		{
			case Piece::Knight:
				m_Piece = 'n';
				break;

			case Piece::Bishop:
				m_Piece = 'b';
				break;

			case Piece::Rook:
				m_Piece = 'r';
				break;

			case Piece::Queen:
				m_Piece = 'q';
				break;
		}
	}
}

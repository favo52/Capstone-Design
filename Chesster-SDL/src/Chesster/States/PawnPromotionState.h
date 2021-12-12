#pragma once

#include "Chesster/States/State.h"

namespace Chesster
{
	class PawnPromotionState : public State
	{
	public:
		PawnPromotionState(StateStack& stack, Context context);
		virtual ~PawnPromotionState();

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

		inline static char *GetPiecePromo() { return &m_Piece; }

	private:
		void SelectOption();

	private:
		enum Piece
		{
			Knight = 0, Bishop, Rook, Queen, Size
		};

	private:
		Window* m_Window;

		SDL_Point m_MousePos;
		SDL_Point m_TouchLocation;

		Texture* m_Figures;
		SDL_Rect m_Background;
		SDL_Rect m_PieceClip[4];
		SDL_Rect m_SelectionHighlight;

		Font m_Font;
		Texture m_TextTexture;

		Piece m_CurrentOption;

		static char m_Piece;
	};
}

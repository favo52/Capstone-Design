#pragma once

#include "State.h"

namespace Chesster
{
	class GameOverState : public State
	{
	public:
		GameOverState(StateStack& stack, Context context);

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		Window* m_Window;
		SDL_Rect m_GameOverOverlay;

		SDL_Point m_MousePos;
	};
}

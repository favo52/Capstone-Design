#pragma once

#include "pch.h"
#include "State.h"
#include "Chesster/Board.h"

namespace Chesster
{
	class GameState : public State
	{
	public:
		GameState(StateStack& stack, Context context);

		bool HandleEvent(SDL_Event& event) override;
		bool Update(const std::chrono::duration<double>& dt) override;
		void Draw() override;

	private:
		Board m_Board;
	};
}

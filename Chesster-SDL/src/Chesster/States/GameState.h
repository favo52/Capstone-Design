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

		void Draw() override;
		bool Update(const std::chrono::duration<double>& dt) override;
		bool HandleEvent(const SDL_Event& event) override;

	private:
		Board m_Board;
	};
}

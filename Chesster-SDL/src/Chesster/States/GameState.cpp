#include "pch.h"
#include "GameState.h"

namespace Chesster
{
	GameState::GameState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Board{ context.window->get() }
	{
	}

	bool GameState::HandleEvent(SDL_Event& event)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					RequestStackPush(StateID::Pause);
			} break;
		}

		m_Board.HandleEvent(event);

		return true;
	}

	bool GameState::Update(const std::chrono::duration<double>& dt)
	{
		m_Board.Update(dt);

		return true;
	}

	void GameState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 255u, 255u, 255u, 255u);
		m_Board.Draw();
	}
}

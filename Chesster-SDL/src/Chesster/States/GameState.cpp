#include "pch.h"
#include "GameState.h"

namespace Chesster
{
	GameState::GameState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Board{ context.window->get() }
	{
	}

	void GameState::Draw()
	{
		SDL_SetRenderDrawColor(Window::Renderer, 255u, 255u, 255u, 255u);
		m_Board.Draw();
	}

	bool GameState::Update(const std::chrono::duration<double>& dt)
	{
		m_Board.Update(dt);

		return true;
	}

	bool GameState::HandleEvent(const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					RequestStackPush(StateID::Pause);
					break;
			}
		}

		m_Board.HandleEvent(event);

		return true;
	}
}

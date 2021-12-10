#include "pch.h"
#include "GameOverState.h"

namespace Chesster
{
	GameOverState::GameOverState(StateStack& stack, Context context) :
		State{ stack, context },
		m_Window{ context.window->get() },
		m_GameOverOverlay{ 0u, 0u, static_cast<int>(m_Window->GetWidth()), static_cast<int>(m_Window->GetHeight()) },
		m_MousePos{}
	{
	}

	bool GameOverState::HandleEvent(SDL_Event& event)
	{
		return false;
	}

	bool GameOverState::Update(const std::chrono::duration<double>& dt)
	{
		return false;
	}

	void GameOverState::Draw()
	{
	}
}

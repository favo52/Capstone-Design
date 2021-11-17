#pragma once

#include "State.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace Pangu
{
	class GameOverState : public State
	{
	public:
		GameOverState(StateStack& stack, Context context);

		void Draw() override;
		bool Update(sf::Time dt) override;
		bool HandleEvent(const sf::Event& event) override;

	private:
		sf::Text m_WinnerText;
		sf::Text m_WinMsgText;
		sf::Time m_ElapsedTime;
	};
}

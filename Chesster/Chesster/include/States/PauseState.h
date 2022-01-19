#pragma once

#include "State.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace Chesster
{
	class PauseState : public State
	{
	public:
		PauseState(StateStack& stack, Context context);

		void Draw() override;
		bool Update(sf::Time dt) override;
		bool HandleEvent(const sf::Event& event) override;

		void UpdateOptionText();

	private:
		enum class PauseOptions
		{
			Continue,
			MainMenu,
			Exit
		};

		friend PauseOptions operator++(PauseOptions& menuOption);
		friend PauseOptions operator--(PauseOptions& menuOption);

	private:
		sf::Sprite m_BackgroundSprite;

		sf::Text m_PausedText;
		std::vector<sf::Text> m_MenuOptions;
		PauseOptions m_CurrentOption;
	};
}

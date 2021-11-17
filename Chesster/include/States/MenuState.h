#pragma once

#include "State.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace Chesster
{
	class MenuState : public State
	{
	public:
		MenuState(StateStack& stack, Context context);

		void Draw() override;
		bool Update(sf::Time dt) override;
		bool HandleEvent(const sf::Event& event) override;

		void UpdateOptionText();

	private:
		enum class MenuOptions
		{
			Play,
			Settings,
			Exit
		};

		friend MenuOptions operator++(MenuOptions& menuOption);
		friend MenuOptions operator--(MenuOptions& menuOption);

	private:
		sf::Sprite m_LogoSprite;

		sf::Text				m_TitleText;
		std::vector<sf::Text>	m_MenuOptions;
		MenuOptions				m_CurrentOption;
	};
}

#pragma once

#include "State.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace Chesster
{
	class TitleState : public State
	{
	public:
		TitleState(StateStack& stack, Context context);

		void Draw() override;
		bool Update(sf::Time dt) override;
		bool HandleEvent(const sf::Event& event) override;

	private:
		sf::Sprite  m_ReadySetGoSprite;
		sf::Sprite  m_MSECSprite;
		sf::Sprite  m_LogoSprite;
		bool m_ShowReadySetCode;
		bool m_ShowMSEC;
		bool m_ShowLogo;

		sf::Text	m_Text;
		bool		m_ShowText;
		sf::Time	m_TextEffectTime;
		sf::Time	m_ElapsedTime;
	};
}

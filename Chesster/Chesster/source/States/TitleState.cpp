#include "States/TitleState.h"
#include "Utility.h"
#include "ResourceHolder.h"

#include <SFML/Graphics/RenderWindow.hpp>

namespace Chesster
{
	TitleState::TitleState(StateStack& stack, Context context) :
		State{ stack, context },
		m_ShowReadySetCode{ true },
		m_ShowMSEC{ false },
		m_ShowLogo{ false },
		m_Text{},
		m_ShowText{ true },
		m_TextEffectTime{ sf::Time::Zero },
		m_ElapsedTime{ sf::Time::Zero }
	{
		sf::Font& font = context.fonts->Get(Fonts::Minecraft);

		// Prepare Ready Set Code logo
		m_ReadySetGoSprite.setTexture(context.textures->Get(Textures::ReadySetCode));
		centerOrigin(m_ReadySetGoSprite);
		m_ReadySetGoSprite.setPosition((context.window->getDefaultView().getSize() / 2.0f) - sf::Vector2f(0.0f, 50.0f));

		//// Prepare MSEC Solutions logo
		//m_MSECSprite.setTexture(context.textures->Get(Textures::MSECGroup));
		//centerOrigin(m_MSECSprite);
		//m_MSECSprite.setPosition((context.window->getDefaultView().getSize() / 2.0f) - sf::Vector2f(0.0f, 50.0f));

		// Prepare Chesster logo
		m_LogoSprite.setTexture(context.textures->Get(Textures::ChessterLogo));
		centerOrigin(m_LogoSprite);
		m_LogoSprite.setPosition((context.window->getDefaultView().getSize() / 2.0f) - sf::Vector2f(0.0f, 120));

		setText(m_Text, font, "Press any key to start", 50u, sf::Color::Black);
		centerOrigin(m_Text);
		m_Text.setPosition(context.window->getSize().x / 2.0f, (context.window->getSize().y / 2.0f) + 300.0f); //getDefaultView().getSize() / 2.0f);
	}

	void TitleState::Draw()
	{
		sf::RenderWindow& window = *GetContext().window;
		//window.clear(sf::Color::White);

		if (m_ShowReadySetCode)
			window.draw(m_ReadySetGoSprite);
		//else if (m_ShowMSEC)
		//	window.draw(m_MSECSprite);			
		else
			window.draw(m_LogoSprite);
			
		if (m_ShowText)
			window.draw(m_Text);
	}

	bool TitleState::Update(sf::Time dt)
	{
		m_TextEffectTime += dt;
		m_ElapsedTime += dt;

		if (m_ElapsedTime > sf::seconds(3))
		{
			m_ShowReadySetCode = false;
			//m_ShowMSEC = true;

		}
		//if (m_ElapsedTime > sf::seconds(6))
		//	m_ShowMSEC = false;

		if (m_TextEffectTime >= sf::seconds(0.5f))
		{
			m_ShowText = !m_ShowText;
			m_TextEffectTime = sf::Time::Zero;
		}

		return true;
	}

	bool TitleState::HandleEvent(const sf::Event& event)
	{
		// If any key is pressed, tigger the next screen
		if (event.type == sf::Event::KeyPressed)
		{
			RequestStackPop();
			RequestStackPush(States::Menu);
		}

		return true;
	}
}

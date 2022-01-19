#include "States/GameOverState.h"
#include "Utility.h"
#include "ResourceHolder.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

namespace Pangu
{
	GameOverState::GameOverState(StateStack& stack, Context context) :
		State{ stack, context },
		m_WinnerText{},
		m_WinMsgText{},
		m_ElapsedTime{ sf::Time::Zero }
	{
		sf::Font& font = context.fonts->Get(Fonts::Minecraft);
		sf::Vector2f windowSize(context.window->getSize());

		m_WinnerText.setFont(font);
		if (*context.toggle == 0)
			m_WinnerText.setString("PLAYER ONE!");
		else
			m_WinnerText.setString("PLAYER TWO!");

		m_WinnerText.setCharacterSize(70u);
		centerOrigin(m_WinnerText);
		m_WinnerText.setPosition(0.5f * windowSize.x, 0.5f * windowSize.y);

		setText(m_WinMsgText, font, "THE WINNER IS...", 70u, sf::Color::White);
		centerOrigin(m_WinMsgText);
		m_WinMsgText.setPosition(m_WinnerText.getPosition() - sf::Vector2f(0.0f, 150.0f));
	}

	void GameOverState::Draw()
	{
		sf::RenderWindow& window = *GetContext().window;
		window.setView(window.getDefaultView());

		// Create dark, semitransparent background
		sf::RectangleShape backgroundShape;
		backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
		backgroundShape.setSize(window.getView().getSize());

		window.draw(backgroundShape);
		window.draw(m_WinnerText);
		window.draw(m_WinMsgText);
	}

	bool GameOverState::Update(sf::Time dt)
	{
		// Show state for 3 seconds, after return to menu
		m_ElapsedTime += dt;
		if (m_ElapsedTime > sf::seconds(4))
		{
			RequestStateClear();
			RequestStackPush(States::Menu);
		}
		return false;
	}

	bool GameOverState::HandleEvent(const sf::Event&)
	{
		return false;
	}
}

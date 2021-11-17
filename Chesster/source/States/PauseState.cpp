#include "States/PauseState.h"
#include "Utility.h"
#include "ResourceHolder.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

namespace Chesster
{
	PauseState::PauseState(StateStack& stack, Context context) :
		State{ stack, context },
		m_BackgroundSprite{},
		m_PausedText{},
		m_MenuOptions{},
		m_CurrentOption{ PauseOptions::Continue }
	{
		sf::Font& font = context.fonts->Get(Fonts::Minecraft);
		sf::Vector2f viewSize = context.window->getView().getSize();

		setText(m_PausedText, font, "GAME PAUSED", 100u, sf::Color::White);
		centerOrigin(m_PausedText);
		m_PausedText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

		sf::Text continueText;
		setText(continueText, font, "CONTINUE", 30u, sf::Color::White);
		centerOrigin(continueText);
		continueText.setPosition(0.5f * viewSize.x, (0.6f * viewSize.y) - 10.0f);
		m_MenuOptions.push_back(continueText);

		sf::Text menuText;
		setText(menuText, font, "MAIN MENU", 30u, sf::Color::White);
		centerOrigin(menuText);
		menuText.setPosition(continueText.getPosition() + sf::Vector2f(0.0f, 50.0f));
		m_MenuOptions.push_back(menuText);

		sf::Text exitText;
		setText(exitText, font, "EXIT", 30u, sf::Color::White);
		centerOrigin(exitText);
		exitText.setPosition(menuText.getPosition() + sf::Vector2f(0.0f, 100.0f));
		m_MenuOptions.push_back(exitText);

		UpdateOptionText();
	}

	void PauseState::Draw()
	{
		sf::RenderWindow& window = *GetContext().window;
		window.setView(window.getDefaultView());

		sf::RectangleShape backgroundShape;
		backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
		backgroundShape.setSize(window.getView().getSize());

		window.draw(backgroundShape);
		window.draw(m_PausedText);
		for (const sf::Text& text : m_MenuOptions)
			window.draw(text);
	}

	bool PauseState::Update(sf::Time dt)
	{
		return false;
	}

	bool PauseState::HandleEvent(const sf::Event& event)
	{
		if (event.type != sf::Event::KeyPressed)
			return false;

		switch (event.key.code)
		{
		case sf::Keyboard::Return:
		{
			if (m_CurrentOption == PauseOptions::Continue)
			{
				RequestStackPop();
			}
			else if (m_CurrentOption == PauseOptions::MainMenu)
			{
				RequestStateClear();
				RequestStackPush(States::Menu);
			}
			else if (m_CurrentOption == PauseOptions::Exit)
			{
				sf::RenderWindow& window = *GetContext().window;
				window.close();
			}
		} break;
		case sf::Keyboard::Escape:
			RequestStackPop();
			break;
		case sf::Keyboard::Up:
		{
			--m_CurrentOption;
			UpdateOptionText();
		} break;
		case sf::Keyboard::Down:
		{
			++m_CurrentOption;
			UpdateOptionText();
		} break;
		}

		return false;
	}

	void PauseState::UpdateOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		// White all texts
		for (sf::Text& text : m_MenuOptions)
			text.setFillColor(sf::Color::White);

		// Red the selected text
		m_MenuOptions[(int)m_CurrentOption].setFillColor(sf::Color::Red);
	}

	PauseState::PauseOptions operator++(PauseState::PauseOptions& pauseOption)
	{
		return pauseOption = ((pauseOption == PauseState::PauseOptions::Exit) ?
			PauseState::PauseOptions::Continue :
			PauseState::PauseOptions((int)pauseOption + 1));
	}

	PauseState::PauseOptions operator--(PauseState::PauseOptions& pauseOption)
	{
		return pauseOption = ((pauseOption == PauseState::PauseOptions::Continue) ?
			PauseState::PauseOptions::Exit :
			PauseState::PauseOptions((int)pauseOption - 1));
	}
}

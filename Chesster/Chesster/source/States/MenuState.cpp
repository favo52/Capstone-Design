#include "States/MenuState.h"
#include "Utility.h"
#include "ResourceHolder.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

namespace Chesster
{
	MenuState::MenuState(StateStack& stack, Context context) :
		State{ stack, context },
		m_TitleText{},
		m_MenuOptions{},
		m_CurrentOption{ MenuOptions::Play }
	{
		sf::Texture& texture = context.textures->Get(Textures::ChessterLogo);
		sf::Font& font = context.fonts->Get(Fonts::Minecraft);

		m_LogoSprite.setTexture(texture);
		centerOrigin(m_LogoSprite);
		m_LogoSprite.setPosition((context.window->getDefaultView().getSize() / 2.0f) - sf::Vector2f(0.0f, 120));

		setText(m_TitleText, font, "CHESSTER", 100u, sf::Color::Black);
		centerOrigin(m_TitleText);
		m_TitleText.setPosition((context.window->getView().getSize() / 2.0f) - sf::Vector2f(0.0f, 300));

		sf::Text playOption;
		setText(playOption, font, "PLAY", 30u, sf::Color::Black);
		centerOrigin(playOption);
		playOption.setPosition((context.window->getView().getSize() / 2.0f) + sf::Vector2f(0.0f, 150));
		m_MenuOptions.push_back(playOption);

		sf::Text SettingsOption;
		setText(SettingsOption, font, "SETTINGS", 30u, sf::Color::Black);
		centerOrigin(SettingsOption);
		SettingsOption.setPosition(playOption.getPosition() + sf::Vector2f(0.0f, 50.0f));
		m_MenuOptions.push_back(SettingsOption);

		sf::Text exitOption;
		setText(exitOption, font, "EXIT", 30u, sf::Color::Black);
		centerOrigin(exitOption);
		exitOption.setPosition(SettingsOption.getPosition() + sf::Vector2f(0.0f, 90.0f));
		m_MenuOptions.push_back(exitOption);

		UpdateOptionText();
	}

	void MenuState::Draw()
	{
		sf::RenderWindow& window = *GetContext().window;

		window.draw(m_LogoSprite);
		window.draw(m_TitleText);

		for (const sf::Text& text : m_MenuOptions)
			window.draw(text);
	}

	bool MenuState::Update(sf::Time dt)
	{
		return true;
	}

	bool MenuState::HandleEvent(const sf::Event& event)
	{
		if (event.type != sf::Event::KeyPressed)
			return false;

		switch (event.key.code)
		{
			case sf::Keyboard::Return:
			{
				if (m_CurrentOption == MenuOptions::Play)
				{
					RequestStackPop();
					RequestStackPush(States::Gameplay);
				}
				else if (m_CurrentOption == MenuOptions::Settings)
				{
					// TODO: Implement a SettingsState
					// SettingsState allows the user to adjust the difficulty level of the engine
					
					//RequestStackPush(States::SettingsState);
				}
				else if (m_CurrentOption == MenuOptions::Exit)
				{
					// By removing itself, the stack will be empty,
					// and the application will know it is time to close.
					RequestStackPop();
				}
			} break;
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

		return true;
	}

	void MenuState::UpdateOptionText()
	{
		if (m_MenuOptions.empty())
			return;

		// White all texts
		for (sf::Text& text : m_MenuOptions)
			text.setFillColor(sf::Color::Black);

		// Red the selected text
		m_MenuOptions[(int)m_CurrentOption].setFillColor(sf::Color::Red);
	}
	
	MenuState::MenuOptions operator++(MenuState::MenuOptions& menuOption)
	{
		return menuOption = ((menuOption == MenuState::MenuOptions::Exit) ?
			MenuState::MenuOptions::Play :
			MenuState::MenuOptions((int)menuOption + 1));
	}

	MenuState::MenuOptions operator--(MenuState::MenuOptions& menuOption)
	{
		return menuOption = ((menuOption == MenuState::MenuOptions::Play) ?
			MenuState::MenuOptions::Exit :
			MenuState::MenuOptions((int)menuOption - 1));
	}
}

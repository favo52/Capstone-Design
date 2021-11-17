#include "Utility.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

#include <cmath>

namespace Chesster
{
	void setText(sf::Text& text, const sf::Font& font, const std::string& str, unsigned int size, sf::Color color)
	{
		text.setFont(font);
		text.setString(str);
		text.setCharacterSize(size);
		text.setFillColor(color);
	}

	void centerOrigin(sf::RectangleShape& rect)
	{
		sf::FloatRect bounds = rect.getLocalBounds();
		rect.setOrigin(std::floor(bounds.left + bounds.width / 2.0f), std::floor(bounds.top + bounds.height / 2.0f));
	}

	void centerOrigin(sf::CircleShape& circle)
	{
		circle.setOrigin(std::floor(circle.getRadius()), std::floor(circle.getRadius()));
	}

	void centerOrigin(sf::Sprite& sprite)
	{
		sf::FloatRect bounds = sprite.getLocalBounds();
		sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.0f), std::floor(bounds.top + bounds.height / 2.0f));
	}

	void centerOrigin(sf::Text& text)
	{
		sf::FloatRect bounds = text.getLocalBounds();
		text.setOrigin(std::floor(bounds.left + bounds.width / 2.0f), std::floor(bounds.top + bounds.height / 2.0f));
	}
}

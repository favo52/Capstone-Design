#pragma once

#include <string>

// Forward declaration of SFML classes
namespace sf
{
	class RectangleShape;
	class CircleShape;
	class Sprite;
	class Text;
	class Font;
	class Color;
}

namespace Chesster
{	
	void setText(sf::Text& text, const sf::Font& font, const std::string& str, unsigned int size, sf::Color color);

	void centerOrigin(sf::RectangleShape& rect);
	void centerOrigin(sf::CircleShape& circle);
	void centerOrigin(sf::Sprite& sprite);
	void centerOrigin(sf::Text& text);
}

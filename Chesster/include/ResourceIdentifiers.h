#pragma once

// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
}

namespace Chesster
{
	enum class Textures
	{
		Board,
		Pieces,
		ChessterLogo,
		ReadySetCode,
		MSECGroup
	};

	enum class Fonts
	{
		Minecraft
	};

	// Forward declaration and a few type definitions
	template<typename Resource, typename Identifiers>
	class ResourceHolder;

	typedef ResourceHolder<sf::Texture, Textures> TextureHolder;
	typedef ResourceHolder<sf::Font, Fonts>		  FontHolder;
}

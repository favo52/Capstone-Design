#pragma once

#include "State.h"
#include "Connector.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <time.h>
#include <string>

namespace Chesster
{
	class GameState : public State
	{
	public:
		GameState(StateStack& stack, Context context);

		void Draw() override;
		bool Update(sf::Time dt) override;
		bool HandleEvent(const sf::Event& event) override;

	private:
		void HandlePlayerInput(const sf::Keyboard::Key& key, const bool& isPressed);

		void LoadPositions();

		std::string ToChessNotation(const sf::Vector2f& position);
		sf::Vector2f ToCoord(char a, char b);

		void Move(const std::string& notation);

	private:
		int m_Board[8][8] = {
			-1, -2, -3, -4, -5, -3, -2, -1,
			-6, -6, -6, -6, -6, -6, -6, -6,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 6,  6,  6,  6,  6,  6,  6,  6,
			 1,  2,  3,  4,  5,  3,  2,  1
		};

	private:
		sf::Sprite m_BoardSprite;
		sf::FloatRect m_Bounds;

		sf::Sprite m_Pieces[32]; // 32 chess pieces
		std::string m_ChessPosition;
		int m_PieceSize;

		bool m_isMove;
		float m_Dx, m_Dy;
		sf::Vector2f m_oldPos, m_newPos;
		std::string m_Str;
		int m_PieceIndex;

		sf::Vector2i m_MousePos;
		sf::Vector2f m_BoardOffset;
		float m_PieceOffset;

		bool m_isComputerTurn;
		unsigned int m_Turn;

		Connector m_Connector;
	};
}

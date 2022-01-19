#include "States/GameState.h"
#include "Utility.h"

#include "ResourceHolder.h"
#include <iostream>
#include <time.h>
#include <SFML/Graphics/RenderWindow.hpp>

namespace Chesster
{
    constexpr int TOTAL_PIECES{ 32 };

    GameState::GameState(StateStack& stack, Context context) :
        State{ stack, context },
        m_Pieces{ nullptr },
        m_PieceSize{ 80 },
        m_isMove{ false },
        m_Dx{ 0.0f },
        m_Dy{ 0.0f },
        m_oldPos{},
        m_newPos{},
        m_Str{ "" },
        m_PieceIndex{ 0 },
        m_ChessPosition{ "" },
        m_BoardOffset{ 23.0f, 23.0f }, // 345 103 -- 22 22
        m_PieceOffset{ 1.06f },
        m_MousePos{},
        m_Connector{},
        m_isComputerTurn{ false },
        m_Turn{ 1 }
	{
        // Connect to Stockfish engine
        wchar_t path[] = L"resources/engines/stockfish_14_x64_avx2.exe";
        m_Connector.ConnectToEngine(path);

        // Set font
        sf::Font& font = context.fonts->Get(Fonts::Minecraft);
        
        // Prepare board
		sf::Texture& boardTexture = context.textures->Get(Textures::Board);
		m_BoardSprite.setTexture(boardTexture);
        m_Bounds = m_BoardSprite.getLocalBounds();

        // Prepare pieces
        m_Pieces = new sf::Sprite[TOTAL_PIECES];
        sf::Texture& pieceTexture = context.textures->Get(Textures::Pieces);
        for (int i = 0; i < TOTAL_PIECES; i++)
            m_Pieces[i].setTexture(pieceTexture);

        LoadPositions();
	}

    GameState::~GameState()
    {
        delete[] m_Board;
        delete[] m_Pieces;
    }

	void GameState::Draw()
	{
		sf::RenderWindow& window = *GetContext().window;
        window.clear(sf::Color(44, 53, 57));

		window.draw(m_BoardSprite);

        for (int i = 0; i < TOTAL_PIECES; ++i)
            m_Pieces[i].move(m_BoardOffset);

        for (int i = 0; i < TOTAL_PIECES; ++i)
            window.draw(m_Pieces[i]);

        window.draw(m_Pieces[m_PieceIndex]);

        for (int i = 0; i < TOTAL_PIECES; ++i)
            m_Pieces[i].move(-m_BoardOffset);
	}

	bool GameState::Update(sf::Time dt)
	{
        // Computer move
        if (m_isComputerTurn)
        {
            // Get stockfish's next move
            m_Str = m_Connector.getNextMove(m_ChessPosition);
            if (m_Str == "error")
                return true;

            m_oldPos = ToCoord(m_Str[0], m_Str[1]);
            m_newPos = ToCoord(m_Str[2], m_Str[3]);

            for (int i = 0; i < TOTAL_PIECES; ++i)
                if (m_Pieces[i].getPosition() / m_PieceOffset == m_oldPos)
                    m_PieceIndex = i;

            sf::RenderWindow& window = *GetContext().window;
            
            // Animation
            for (sf::Time elapsedTime = sf::Time::Zero; elapsedTime < sf::seconds(1); elapsedTime += dt)
            {
                sf::Vector2f position = m_newPos - m_oldPos;
                m_Pieces[m_PieceIndex].move(position.x * dt.asSeconds(), position.y * dt.asSeconds());
                window.draw(m_BoardSprite);

                for (int i = 0; i < TOTAL_PIECES; ++i)
                    m_Pieces[i].move(m_BoardOffset);

                for (int i = 0; i < TOTAL_PIECES; ++i)
                    window.draw(m_Pieces[i]);

                window.draw(m_Pieces[m_PieceIndex]);

                for (int i = 0; i < TOTAL_PIECES; ++i)
                    m_Pieces[i].move(-m_BoardOffset);

                window.display();
            }

            Move(m_Str);
            m_ChessPosition += m_Str + " ";

            m_Pieces[m_PieceIndex].setPosition(m_newPos * m_PieceOffset);
            m_isComputerTurn = false;

            // Print entire game so far
            std::cout << '\n' << m_Turn << ". " << m_ChessPosition << ' ';
            ++m_Turn;
        }

        if (m_isMove) // dragging a piece
            m_Pieces[m_PieceIndex].setPosition(m_MousePos.x - m_Dx, m_MousePos.y - m_Dy);

		return true;
	}

	bool GameState::HandleEvent(const sf::Event& event)
	{
        sf::RenderWindow& window = *GetContext().window;
        m_MousePos = sf::Mouse::getPosition(window) - sf::Vector2i(m_BoardOffset);

        switch (event.type)
        {
            case sf::Event::KeyPressed:
            {
                // Move back
                if (event.key.code == sf::Keyboard::BackSpace)
                {
                    if (m_ChessPosition.length() > 6)
                        m_ChessPosition.erase(m_ChessPosition.length() - 6, 5);

                    LoadPositions();
                }

                // Pause screen
                if (event.key.code == sf::Keyboard::Escape)
                    RequestStackPush(States::Pause);

                HandlePlayerInput(event.key.code, true);
            } break;

            case sf::Event::KeyReleased:
            {
                HandlePlayerInput(event.key.code, false);
            } break;

            case sf::Event::MouseButtonPressed:
            {
                // Drag and drop
                if (event.key.code == sf::Mouse::Left)
                    for (int i = 0; i < TOTAL_PIECES; i++)
                        if (m_Pieces[i].getGlobalBounds().contains(m_MousePos.x, m_MousePos.y))
                        {
                            m_isMove = true;
                            m_PieceIndex = i;

                            m_Dx = m_MousePos.x - m_Pieces[i].getPosition().x;
                            m_Dy = m_MousePos.y - m_Pieces[i].getPosition().y;
                            m_oldPos = m_Pieces[i].getPosition();
                        }
            } break;

            case sf::Event::MouseButtonReleased:
            {
                if (event.key.code == sf::Mouse::Left)
                {
                    m_isMove = false;
                    sf::Vector2f position = m_Pieces[m_PieceIndex].getPosition() + sf::Vector2f(m_PieceSize / 2.0f, m_PieceSize / 2.0f);
                    m_newPos = sf::Vector2f(m_PieceSize * int(position.x / m_PieceSize) * m_PieceOffset, m_PieceSize * int(position.y / m_PieceSize) * m_PieceOffset);
                    m_Str = ToChessNotation(m_oldPos) + ToChessNotation(m_newPos);
                    
                    // Print move
                    //std::cout << m_Turn << ". " << m_ChessPosition << '\n';

                    Move(m_Str); // remove piece

                    if (m_oldPos != m_newPos)
                        m_ChessPosition += m_Str + " ";

                    m_Pieces[m_PieceIndex].setPosition(m_newPos);
                }
            } break;
        } // end event.type

		return true;
	}

    void GameState::HandlePlayerInput(const sf::Keyboard::Key& key, const bool& isPressed)
    {
        switch (key)
        {
        case sf::Keyboard::Space:
            m_isComputerTurn = isPressed;
            break;
        default:
            break;
        } // end event.key.code
    }

    void GameState::LoadPositions()
    {
        constexpr int RANK{ 8 };
        int pieceIndex{ 0 };

        for (int y = 0; y < RANK; y++)
            for (int x = 0; x < RANK; x++)
            {
                int boardSquare = m_Board[x + y * RANK];
                if (!boardSquare)
                    continue;

                int xPos = abs(boardSquare) - 1;
                int yPos = (boardSquare > 0) ? 1 : 0;

                // Set textures from image
                m_Pieces[pieceIndex].setTextureRect(sf::IntRect(m_PieceSize * xPos, m_PieceSize * yPos, m_PieceSize, m_PieceSize));
                m_Pieces[pieceIndex].setPosition((m_PieceSize * x * m_PieceOffset), (m_PieceSize * y * m_PieceOffset));
                ++pieceIndex;
            }

        for (int i = 0; i < m_ChessPosition.length(); i += 5) // 5 because "d2d4" (for example) + whitespace
            Move(m_ChessPosition.substr(i, 4));
    }

    std::string GameState::ToChessNotation(const sf::Vector2f& position)
    {
        std::string notation{ "" };

        notation += char(m_PieceOffset *  position.x / m_PieceSize + int('a'));
        notation += char(m_PieceOffset * 7 - position.y / m_PieceSize + int('1'));

        return notation;
    }

    sf::Vector2f GameState::ToCoord(char a, char b)
    {
        int x = int(a) - int('a');
        int y = 7 - int(b) + int('1');

        return sf::Vector2f(x * m_PieceSize, y * m_PieceSize);
    }

    void GameState::Move(const std::string& notation)
    {
        sf::Vector2f oldPos = ToCoord(notation[0], notation[1]);
        sf::Vector2f newPos = ToCoord(notation[2], notation[3]);

        for (int i = 0; i < TOTAL_PIECES; i++)
            if ((m_Pieces[i].getPosition()) / m_PieceOffset == newPos)
                m_Pieces[i].setPosition(-100.0f, -100.0f);

        for (int i = 0; i < TOTAL_PIECES; i++)
            if (m_Pieces[i].getPosition() / m_PieceOffset == oldPos)
                m_Pieces[i].setPosition(newPos * m_PieceOffset);

        // Castling             // If king has not been moved
        if (notation == "e1g1") if (m_ChessPosition.find("e1") == std::string::npos)
            {
                Move("h1f1");
                //m_ChessPosition += std::string("h1f1 0000 ");
            };
        if (notation == "e8g8") if (m_ChessPosition.find("e8") == std::string::npos)
        {
            Move("h8f8");
            //m_ChessPosition += std::string("0000") + " h8f8 ";
        }
        if (notation == "e1c1") if (m_ChessPosition.find("e1") == std::string::npos)
        {
            Move("a1d1");
            //m_ChessPosition += std::string("a1d1") + " ";
        }
        if (notation == "e8c8") if (m_ChessPosition.find("e8") == std::string::npos)
        {
            Move("a8d8");
            //m_ChessPosition += std::string("a8d8") + " ";
        }
    }
}

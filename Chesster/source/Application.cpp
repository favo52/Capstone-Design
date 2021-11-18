#include "Application.h"
#include "Utility.h"
#include "States/TitleState.h"
#include "States/MenuState.h"
#include "States/GameState.h"
#include "States/PauseState.h"
//#include "States/GameOverState.h"

#include "SFML/Window/Event.hpp"

namespace Chesster
{
	const sf::Time Application::TimePerFrame = sf::seconds(1.0f / 60.0f);
	constexpr unsigned int WINDOW_WIDTH{ 720 };
	constexpr unsigned int WINDOW_HEIGHT{ 720 };

	/*****************************************************
	*	Constructor
	*****************************************************/
	Application::Application() :
		m_Window{ sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chesster!", sf::Style::Close },
		m_Textures{},
		m_Fonts{},
		m_StateStack{ State::Context(m_Window, m_Textures, m_Fonts) },
		m_FPSText{},
		m_FPSUpdateTime{},
		m_FPSNumFrames{ 0 }
	{
		m_Window.setKeyRepeatEnabled(false);
		m_Window.setVerticalSyncEnabled(true);
		
		// Load font
		m_Fonts.Load(Fonts::Minecraft, "resources/fonts/Minecraft.ttf");

		// Load splash screen logos
		m_Textures.Load(Textures::ReadySetCode, "resources/textures/ReadySetCode.jpeg");
		m_Textures.Load(Textures::MSECGroup, "resources/textures/MSEC.jpeg");
		m_Textures.Load(Textures::ChessterLogo, "resources/textures/ChessterLogo.jpeg");
		
		// Load board and pieces
		m_Textures.Load(Textures::Board, "resources/textures/2ChessBoard_Notations.png");
		m_Textures.Load(Textures::Pieces, "resources/textures/ChessPieces.png");

		setText(m_FPSText, m_Fonts.Get(Fonts::Minecraft), " ", 10u, sf::Color::Black);
		m_FPSText.setPosition(5.0f, 5.0f);

		RegisterStates();
		m_StateStack.PushState(States::Splashscreen);
	}

	/*****************************************************
	*	Destructor
	*****************************************************/
	Application::~Application()
	{
	}

	/*****************************************************
	*	Public Functions
	*****************************************************/
	void Application::Run()
	{
		sf::Clock clock;
		sf::Time elapsedTime = sf::Time::Zero;

		while (m_Window.isOpen())
		{
			sf::Time dt = clock.restart();
			elapsedTime += dt;
			while (elapsedTime > TimePerFrame)
			{
				elapsedTime -= TimePerFrame;

				ProcessInput();
				Update(TimePerFrame);

				// Check inside this loop, because stack might be empty before update() call
				if (m_StateStack.IsEmpty())
					m_Window.close();
			}

			UpdateFPSCounter(dt);
			Render();
		}

		//CloseConnection();
	}

	/*****************************************************
	*	Private Functions
	*****************************************************/

	void Application::ProcessInput()
	{
		sf::Event e;
		while (m_Window.pollEvent(e))
		{
			m_StateStack.HandleEvent(e);

			if (e.type == sf::Event::Closed)
				m_Window.close();
		}
	}

	void Application::Update(sf::Time dt)
	{
		m_StateStack.Update(dt);
	}

	void Application::Render()
	{
		m_Window.clear(sf::Color::White);

		m_StateStack.Draw();

		m_Window.draw(m_FPSText);

		m_Window.display();
	}

	void Application::UpdateFPSCounter(sf::Time dt)
	{
		m_FPSUpdateTime += dt;
		m_FPSNumFrames += 1;
		if (m_FPSUpdateTime >= sf::seconds(1.0f))
		{
			m_FPSText.setString("FPS: " + std::to_string(m_FPSNumFrames));

			m_FPSUpdateTime -= sf::seconds(1.0f);
			m_FPSNumFrames = 0;
		}
	}
	void Application::RegisterStates()
	{
		m_StateStack.RegisterState<TitleState>(States::Splashscreen);
		m_StateStack.RegisterState<MenuState>(States::Menu);
		m_StateStack.RegisterState<GameState>(States::Gameplay);
		m_StateStack.RegisterState<PauseState>(States::Pause);
		//m_StateStack.RegisterState<GameOverState>(States::Gameover);
	}
}

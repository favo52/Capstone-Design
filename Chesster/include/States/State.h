#pragma once

#include "StateIdentifiers.h"
#include "ResourceIdentifiers.h"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

// Forward declaration of SFML classes
namespace sf
{
	class RenderWindow;
}

namespace Chesster
{
	class Paddle;
	class StateStack;

	class State
	{
	public:
		typedef std::unique_ptr<State> StatePtr;

		struct Context
		{
			Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts);
			sf::RenderWindow* window;
			TextureHolder* textures;
			FontHolder* fonts;
		};

	public:
		State(StateStack& stack, Context context);
		virtual ~State();

		virtual void Draw() = 0;
		virtual bool Update(sf::Time dt) = 0;
		virtual bool HandleEvent(const sf::Event& event) = 0;

	protected:
		void RequestStackPush(States stateID);
		void RequestStackPop();
		void RequestStateClear();

		Context GetContext() const;

	private:
		StateStack* m_Stack;
		Context		m_Context;
	};
}

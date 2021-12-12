#pragma once

#include "pch.h"
#include "Chesster/Core/Window.h"
#include "Chesster/Core/ResourceHolder.h"

namespace Chesster
{
	class StateStack;

	enum class StateID
	{
		None,
		Title,
		Menu,
		Settings,
		Gameplay,
		Pause,
		Gameover,
		PawnPromo
	};

	class State
	{
	public:
		typedef std::unique_ptr<State> StatePtr;

		struct Context
		{
			Context(std::unique_ptr<Window>& window, TextureHolder& textures, FontHolder& fonts);
			std::unique_ptr<Window>* window;
			TextureHolder* textures;
			FontHolder* fonts;
		};

	public:
		State(StateStack& stack, Context context);
		virtual ~State();

		virtual bool HandleEvent(SDL_Event& e) = 0;
		virtual bool Update(const std::chrono::duration<double>& dt) = 0;
		virtual void Draw() = 0;

	protected:
		void RequestStackPush(StateID stateID);
		void RequestStackPop();
		void RequestStateClear();

		Context GetContext() const;

	private:
		StateStack* m_Stack;
		Context		m_Context;
	};
}

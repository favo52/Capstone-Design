#pragma once

#include "pch.h"
#include "Chesster/Window.h"
#include "Chesster/ResourceHolder.h"

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
		Gameover
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

		virtual void Draw() = 0;
		virtual bool Update(const std::chrono::duration<double>& dt) = 0;
		virtual bool HandleEvent(const SDL_Event& e) = 0;

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

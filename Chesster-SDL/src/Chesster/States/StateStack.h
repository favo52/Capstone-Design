#pragma once

#include "State.h"

namespace Chesster
{
	class StateStack
	{
	public:
		enum class Action
		{
			Push,
			Pop,
			Clear,
		};

	public:
		explicit StateStack(State::Context context);

		template<typename T>
		void RegisterState(StateID stateID);

		void HandleEvent(SDL_Event& event);
		void Update(const std::chrono::duration<double>& dt);
		void Draw();

		void PushState(StateID stateID);
		void PopState();
		void ClearStates();

		bool IsEmpty() const;

		void ApplyPendingChanges();

	private:
		State::StatePtr CreateState(StateID stateID);

	private:
		struct PendingChange
		{
			explicit PendingChange(Action action, StateID stateID = StateID::None);

			Action action;
			StateID stateID;
		};

	private:
		std::vector<State::StatePtr> m_Stack;
		std::vector<PendingChange> m_PendingList;

		State::Context m_Context;
		std::map<StateID, std::function<State::StatePtr()>> m_Factories;
	};

	template<typename T>
	void StateStack::RegisterState(StateID stateID)
	{
		m_Factories[stateID] = [this]()
		{
			return State::StatePtr(new T(*this, m_Context));
		};
	}
}

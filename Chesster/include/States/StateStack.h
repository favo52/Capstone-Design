#pragma once

#include "State.h"
#include "StateIdentifiers.h"
#include "ResourceIdentifiers.h"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <utility>
#include <functional>
#include <map>

// Forward declaration of SFML classes
namespace sf
{
	class Event;
	class RenderWindow;
}

namespace Chesster
{
	class StateStack : private sf::NonCopyable
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
		void RegisterState(States stateID);

		void Update(sf::Time dt);
		void Draw();
		void HandleEvent(const sf::Event& event);

		void PushState(States stateID);
		void PopState();
		void ClearStates();

		bool IsEmpty() const;

	private:
		State::StatePtr CreateState(States stateID);
		void ApplyPendingChanges();

	private:
		struct PendingChange
		{
			explicit PendingChange(Action action, States stateID = States::None);

			Action action;
			States stateID;
		};

	private:
		std::vector<State::StatePtr> m_Stack;
		std::vector<PendingChange> m_PendingList;

		State::Context m_Context;
		std::map<States, std::function<State::StatePtr()>> m_Factories;
	};

	template<typename T>
	void StateStack::RegisterState(States stateID)
	{
		m_Factories[stateID] = [this]()
		{
			return State::StatePtr(new T(*this, m_Context));
		};
	}
}

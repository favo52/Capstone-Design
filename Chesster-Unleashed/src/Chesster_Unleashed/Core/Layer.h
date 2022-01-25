#pragma once

#include <string>
#include <chrono>

#include "Chesster_Unleashed/Core/Core.h"

#include "SDL_events.h"

namespace Chesster
{
	class Layer
	{
	public:
		Layer(const std::string& debugName = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(SDL_Event& event) {}
		virtual void OnUpdate(const std::chrono::duration<double>& dt) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}

		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}

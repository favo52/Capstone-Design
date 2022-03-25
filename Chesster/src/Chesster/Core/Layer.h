#pragma once

#include "SDL_events.h"

namespace Chesster
{
	/// <summary>
	/// Base class to be inherited by the Title, Game, and ImGui layers.
	/// </summary>
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

		/// <summary>
		/// 
		/// </summary>
		/// <returns>A std::string of the name of the layer.</returns>
		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName; // Used to identify the layer while debugging
	};
}

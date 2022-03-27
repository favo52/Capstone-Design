#pragma once

#include "SDL_events.h"

namespace Chesster
{
	/*	Base class to be inherited by the Title, Game, and ImGui layers. */
	class Layer
	{
	public:
		/** 
		 @param debugName The name of the Layer. */
		Layer(const std::string& debugName = "Layer");

		/*	Default destructor */
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(SDL_Event& event) {}
		virtual void OnUpdate(const std::chrono::duration<double>& dt) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}

		/** Retrieves the Layer's name used for debugging.
		 @return A std::string of the name of the Layer. */
		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName; // Used to identify the layer while debugging
	};
}

#pragma once

#include "SDL_events.h"

namespace Chesster
{
	/*	Base class to be inherited by the Title, Game, and ImGui layers.
		It is not a pure virtual class to let the Layers decide what function to override. */
	class Layer
	{
	public:
		/** Constructor. Assigns a debug name to the Layer.
		 @param debugName The name of the Layer. */
		Layer(const std::string& debugName = "Layer");

		/*	Default destructor */
		virtual ~Layer() = default;

		/*	Performs the code when a Layer is pushed to the LayerStack. Similar to a constructor. */
		virtual void OnAttach() {}

		/*	Performs the code when a Layer is popped from the LayerStack. Similar to a destructor. */
		virtual void OnDetach() {}

		/**	Handles all SDL events.
		 @param event The SDL_Event. */
		virtual void OnEvent(SDL_Event& event) {}

		/** Updates the application's logic.
		 @param dt The delta time used for animations. */
		virtual void OnUpdate(const std::chrono::duration<double>& dt) {}

		/*	Draws to the screen. */
		virtual void OnRender() {}

		/*	Draws the ImGui graphical user interface. */
		virtual void OnImGuiRender() {}

		/** Retrieves the Layer's name used for debugging.
		 @return A std::string of the name of the Layer. */
		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName; // Used to identify the layer while debugging
	};
}

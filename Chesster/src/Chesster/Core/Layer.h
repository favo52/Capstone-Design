#pragma once

#include "SDL_events.h"

namespace Chesster
{
	/*	Base class to be inherited by the Title, Game, and ImGui layers.
		It is not a pure virtual class to let the Layers decide what function to override. */
	class Layer
	{
	public:
		enum class Action { None, PushLayer, PopLayer };

	public:
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
	};
}

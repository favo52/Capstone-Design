/*
* Copyright 2022-present, Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#pragma once

#include "SDL_events.h"

namespace Chesster
{
	using Duration = std::chrono::duration<double>;
	using Timestep = std::chrono::duration<double>;

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
		virtual void OnUpdate(const Timestep& dt) {}

		/*	Draws to the screen. */
		virtual void OnRender() {}

		/*	Draws the ImGui graphical user interface. */
		virtual void OnImGuiRender() {}
	};
}

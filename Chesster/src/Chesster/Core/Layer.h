/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
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

#include <SDL_events.h>

namespace Chesster
{
	using Duration = std::chrono::duration<double>;
	using Timestep = std::chrono::duration<double>;

	/* Base class to be inherited by the the different layers in the application.
	   It is not pure virtual to let each Layer decide which functions it needs. */
	class Layer
	{
	public:
		/* The Action done to the Layer when applying changes to the LayerStack. */
		enum class Action { None, PushLayer, PopLayer };

	public:
		/* Default virtual destructor */
		virtual ~Layer() = default;

		/* Runs when the Layer is pushed to the LayerStack. */
		virtual void OnAttach() {}

		/* Runs when the Layer is popped from the LayerStack. */
		virtual void OnDetach() {}
		
		/** Handles all the events specific to the Layer.
		 @param event The pending SDL_Event happening on this Layer. */
		virtual void OnEvent(SDL_Event& event) {}

		/** Updates the physics or logic specific to the Layer.
		 @param dt The delta time used for animations. */
		virtual void OnUpdate(const Timestep& dt) {}

		/* Draws all the Texture images, texts, and/or rectangles specific to the Layer. */
		virtual void OnRender() {}

		/* Draws the ImGui graphical user interface specific to the Layer. */
		virtual void OnImGuiRender() {}
	};
}

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

#include "Chesster/Core/Layer.h"

namespace Chesster
{
	/*	Contains the necesary backend code for drawing the ImGui GUI. */
	class ImGuiLayer : public Layer
	{
	public:
		/*	Default destructor. */
		virtual ~ImGuiLayer() = default;

		/*	Configures ImGui context, style, fonts, and rendering backends. */
		virtual void OnAttach() override;

		/*	Destroys the ImGui implementation and performs cleanup. */
		virtual void OnDetach() override;

		/*	Starts the Dear ImGui frame. Must be called before attempting to draw anything. */
		void Begin();

		/*	Performs the ImGui rendering. */
		void End();

	private:
		/*	Changes the colors of the ImGui style to a darker theme. */
		void SetDarkTheme();
	};
}

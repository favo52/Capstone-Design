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

#include "Chesster/Core/Window.h"
#include "Chesster/Layers/Layer.h"

// Forward declaration
int main(int argc, char* args[]);

namespace Chesster
{
	// Forward declarations
	class TitleLayer;
	class GameLayer;
	class ImGuiLayer;

	/* Creates the Window, handles all the events, and manages the Layers.
	   Contains the main program loop and the LayerStack. */
	class Application
	{
	public:
		/** It initializes the logger, and creates the Window and starting layers.
		 @param name The name of the application. */
		Application(const std::string& name);

		/* Calls OnDetach() on all remaining Layers and empties the LayerStack. */
		virtual ~Application();

		/* Stops the application. */
		void Quit() { m_IsRunning = false; }

		/* Adds the specified PendingChange to the PendingChanges queue. 
		   The change will occur at the OnEvent stage.
		 @param layerAction The action to be performed on the Layer.
		 @param layer The Layer to be pushed or popped. */
		void RequestLayerAction(Layer::Action layerAction, const std::shared_ptr<Layer>& layer);

		/* Retrieves the Window object. A convenience function. 
		   It is used to acquire the Window's properties or the SDL_Window.
		 @return A reference to the Application's Window member variable. */
		Window& GetWindow() { return *m_Window; }

		/* Used to retrieve the instance of this application.
		 @return A reference to this Application object. */
		static Application& Get() { return *s_Instance; }

	private:
		/* Contains the main program loop and calculates the Timestep.
		   It handles events on queue, layer push/pop, updates game logic, 
		   draws everything to back buffer, and updates the screen. */
		void Run();

		/* Handles events on queue. Events include Window events 
		   and mouse/keyboard input.
		 @param sdlEvent The SDL event to handle. */
		void OnEvent(SDL_Event& sdlEvent);

		/* Inserts a Layer into the m_LayerStack and calls its OnAttach() function.
		 @param layer A shared pointer of the Layer to be pushed. */
		void PushLayer(const std::shared_ptr<Layer>& layer);

		/* Erases a Layer from the m_LayerStack and calls its OnDetach() function.
		 @param layer A shared pointer of the Layer to be removed. */
		void PopLayer(const std::shared_ptr<Layer>& layer);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_IsRunning;	// Program main loop continues while true

		std::vector<std::shared_ptr<Layer>> m_LayerStack;
		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

		struct PendingChange
		{
			Layer::Action Action{ Layer::Action::None };
			std::shared_ptr<Layer> Layer;
		};
		std::vector<PendingChange> m_PendingChanges;

	private:
		static Application* s_Instance;				// Pointer to this.

		friend int ::main(int argc, char* args[]);	// Allows main() access to Run()
	};
}

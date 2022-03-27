#pragma once

#include "Chesster/Core/Window.h"
#include "Chesster/Core/LayerStack.h"

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

// Forward declaration
int main(int argc, char* args[]);

namespace Chesster
{
	// Forward declarations
	class TitleLayer;
	class ImGuiLayer;

	/*	The main application class. Creates a Window, handles all the events, and manages the layers. */
	class Application
	{
	public:
		/** It initializes the logger, and creates the Window and layers.
		 @param name The name of the application. */
		Application(const std::string& name);

		/*	Default destructor. */
		virtual ~Application() = default;

		/*	Stops the application. Changes m_Running to false. */
		void Quit() { m_Running = false; }

		/** Handles events on queue. Events include Window events,
			mouse and keyboard input, and 
		 @param sdlEvent The SDL event to handle. */
		void OnEvent(SDL_Event& sdlEvent);

		/*	Handles conditions for Layer pushes and pops happening during runtime. */
		void OnLayerEvent();

		/**	Pushes a Layer into the LayerStack and calls its OnAttach() function.
		 @param layer A pointer of the layer to be pushed. */
		void PushLayer(Layer* layer);

		/**	Emplaces the Layer into the back of the LayerStack and calls its OnAttach() function.
			This Layer will reside after Layers inserted with PushLayer().
		 @param layer A pointer of the layer to be pushed. */
		void PushOverlay(Layer* layer);

		/** Retrieves the Window object. A convenience function. 
			It is used to acquire the window's properties or the SDL_Window.
		 @return A reference to the Application's Window member variable. */
		Window& GetWindow() { return *m_Window; }

		/** Retrieves the ImGuiLayer object. A convenience function. 
			It is used to update and draw the different ImGui Windows.
		 @return A pointer to the Application's ImGuiLayer member variable. */
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		/** It is used to retrieve the instance of the current application.
		 @return A reference to this Application object. */
		static Application& Get() { return *s_Instance; }

	private:
		/*	Contains the main program loop.
			It handles events on queue, layer push/pop, updates game logic,
			draws everything, and updates the screen. */
		void Run();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;

		LayerStack m_LayerStack;

		TitleLayer* m_TitleLayer;
		ImGuiLayer* m_ImGuiLayer;

	private:
		static Application* s_Instance;				// Pointer to this.

		friend int ::main(int argc, char* args[]);	// Allows main() access to Run()
	};
}

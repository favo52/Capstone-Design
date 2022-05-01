#pragma once

#include "Chesster/Core/Window.h"
#include "Chesster/Core/Layer.h"

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

// Forward declaration
int main(int argc, char* args[]);

namespace Chesster
{
	// Forward declarations
	class TitleLayer;
	class GameLayer;
	class ImGuiLayer;

	/*	The main application class. Creates a Window, handles all the events, and manages the layers. */
	class Application
	{
	public:
		/** It initializes the logger, and creates the Window and starting layers.
		 @param name The name of the application. */
		Application(const std::string& name);

		/*	Calls OnDetach() on all remaining Layers and empties the LayerStack. */
		virtual ~Application();

		/*	Stops the application. Changes m_Running to false. */
		void Quit() { m_Running = false; }

		/** Adds the specified LayerChange to the PendingChanges queue.
			The change will occur on the next loop iteration.
		 @param layerAction The action to be performed on the Layer.
		 @param layer The Layer to be pushed or popped. */
		void RequestLayerAction(Layer::Action layerAction, const std::shared_ptr<Layer>& layer);

		/** Retrieves the Window object. A convenience function. 
			It is used to acquire the window's properties or the SDL_Window.
		 @return A reference to the Application's Window member variable. */
		Window& GetWindow() { return *m_Window; }

		/** It is used to retrieve the instance of the current application.
		 @return A reference to this Application object. */
		static Application& Get() { return *s_Instance; }

	private:
		/*	Contains the main program loop.
			It handles events on queue, layer push/pop, updates game logic,
			draws everything, and updates the screen. */
		void Run();

		/** Handles events on queue. Events include Window events,
			mouse and keyboard input, and
		 @param sdlEvent The SDL event to handle. */
		void OnEvent(SDL_Event& sdlEvent);

		/**	Inserts a Layer into the m_LayerStack and calls its OnAttach() function.
		 @param layer A std::shared_ptr of the layer to be pushed. */
		void PushLayer(const std::shared_ptr<Layer>& layer);

		/**	Erases a Layer from the m_LayerStack and calls its OnDetach() function.
		 @param layer A std::shared_ptr of the layer to be erased. */
		void PopLayer(const std::shared_ptr<Layer>& layer);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;

		std::vector<std::shared_ptr<Layer>> m_LayerStack;

		struct PendingChange
		{
			Layer::Action Action{ Layer::Action::None };
			std::shared_ptr<Layer> Layer;
		};
		std::vector<PendingChange> m_PendingChanges;

		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

	private:
		static Application* s_Instance;				// Pointer to this.

		friend int ::main(int argc, char* args[]);	// Allows main() access to Run()
	};
}

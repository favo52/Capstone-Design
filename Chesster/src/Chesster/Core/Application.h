#pragma once

#include "Chesster/Core/Window.h"
#include "Chesster/Core/LayerStack.h"

/* Legend
 * <summary> Beginning of summary
 * </summary> End of summary
*/

// Forward declaration
int main(int argc, char* args[]);

namespace Chesster
{
	// Forward declarations
	class TitleLayer;
	class ImGuiLayer;

	/// <summary>
	/// The main application class. Creates a Window,
	/// handles all the events, and manages the layers.
	/// </summary>
	class Application
	{
	public:
		/// <summary>
		/// Application constructor. It Creates a Window,
		/// and creates and pushes the layers.
		/// </summary>
		/// <param name="name">The name of the application.</param>
		Application(const std::string& name);

		/// <summary>
		/// Application destructor. Releases all the resources.
		/// </summary>
		virtual ~Application() = default;

		/// <summary>
		/// Stops the application. Changes m_Running to false.
		/// </summary>
		void Quit() { m_Running = false; }

		/// <summary>
		/// Handles events on queue.
		/// </summary>
		/// <param name="sdlEvent">The SDL event to handle.</param>
		void OnEvent(SDL_Event& sdlEvent);

		/// <summary>
		/// 
		/// </summary>
		void OnLayerEvent();

		/// <summary>
		/// Used to push a layer.
		/// </summary>
		/// <param name="layer">A pointer to the layer to be pushed.</param>
		void PushLayer(Layer* layer);

		/// <summary>
		/// Used to push an overlay.
		/// </summary>
		/// <param name="layer">A pointer to the layer to be pushed.</param>
		void PushOverlay(Layer* layer);

		/// <summary>
		/// Retrieves the Window object. A convenience function.
		/// It is used to acquire the window's properties.
		/// </summary>
		/// <returns>A reference to the m_Window member variable.</returns>
		Window& GetWindow() { return *m_Window; }

		/// <summary>
		/// Retrieves the ImGuiLayer object. A convenience function.
		/// It is used to update and draw the different ImGui Windows.
		/// </summary>
		/// <returns>A pointer to the m_ImGuiLayer member variable.</returns>
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		/// <summary>
		/// This function is used to retrieve the instance of the current application.
		/// </summary>
		/// <returns>A reference to the application object.</returns>
		static Application& Get() { return *s_Instance; }

	private:
		/// <summary>
		/// Starts the application.
		/// This function contains the main program loop.
		/// </summary>
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

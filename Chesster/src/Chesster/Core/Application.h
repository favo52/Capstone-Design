#pragma once

#include "Chesster/Core/LayerStack.h"
#include "Chesster/Core/ResourceHolder.h"

#include "Chesster/Layers/TitleLayer.h"
#include "Chesster/ImGui/ImGuiLayer.h"

/* Legend
 * <summary> Beginning of summary
 * </summary> End of summary
*/

// Forward declarations
int main(int argc, char* args[]);

namespace Chesster
{
	/// <summary>
	/// The main application class. Creates a Window,
	/// loads all the required resources, and manages the layers.
	/// </summary>
	class Application
	{
	public:
		/// <summary>
		/// Application constructor. It Creates a Window,
		/// loads all the required resources, and creates and pushes the layers.
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
		/// Used to pop a layer after it is not needed anymore.
		/// </summary>
		/// <param name="layer">A pointer to the layer to be popped.</param>
		void PopLayer(Layer* layer);

		/// <summary>
		/// Used to pop an overlay after it is not needed anymore.
		/// </summary>
		/// <param name="layer">A pointer to the layer to be popped.</param>
		void PopOverlay(Layer* layer);

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

	public:
		TextureHolder m_TextureHolder;
		FontHolder m_FontHolder;

	private:
		/// <summary>
		/// Starts the application.
		/// This function contains the main program loop.
		/// </summary>
		void Run();

		/// <summary>
		/// Toggles the window to fullscreen if it is windowed.
		/// Toggles back to window mode if already fullscreen.
		/// </summary>
		void ToggleFullscreen();

		/// <summary>
		/// Handles all window events i.e. window closing, window resize,
		/// window minimize, window maximize.
		/// </summary>
		/// <param name="sdlEvent">The SDL window event.</param>
		void OnWindowEvent(SDL_Event& sdlEvent);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
		bool m_Minimized;
		bool m_Fullscreen;

		LayerStack m_LayerStack;

		TitleLayer* m_TitleLayer;
		ImGuiLayer* m_ImGuiLayer;

	private:
		static Application* s_Instance; // Pointer to this.

		friend int ::main(int argc, char* args[]); // Allows main() access to Run()
	};
}

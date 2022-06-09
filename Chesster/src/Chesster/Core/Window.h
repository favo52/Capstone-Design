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

#include "Chesster/Renderer/Renderer.h"

namespace Chesster
{
	/* Window Properties. It stores the Window's title, width and height. */
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title, uint32_t width, uint32_t height) :
			Title{ title }, Width{ width }, Height{ height } {}
	};

	/* Initializes the SDL library and its subsystems.
	   It creates the window and graphics context for the application. */
	class Window
	{
	public:
		/* Creates an SDL_Window and a graphics context for the application.
		 @param props The Window properties. */
		Window(const WindowProps& props);

		/* Destroys the SDL_Window and Quits all SDL systems. */
		virtual ~Window();

		/* Update the screen with any rendering performed since the previous call. */
		void SwapBuffers();

		/* Handles window closing and window resize events.
		 @param sdlEvent The SDL window event to handle. */
		void OnWindowEvent(SDL_Event& sdlEvent);

		/* Retrieves the Width from the WindowProps.
		 @return The width of the Window. */
		const uint32_t& GetWidth() const { return m_WinProps.Width; }

		/* Retrieves the Height from the WindowProps.
		 @return The height of the Window. */
		const uint32_t& GetHeight() const { return m_WinProps.Height; }

		/* Retrieves the SDL_Window object.
		 @return A pointer to the Window's SDL_Window member variable. */
		SDL_Window* GetSDLWindow() const { return m_Window; }

	private:
		SDL_Window* m_Window;
		WindowProps m_WinProps;

		std::unique_ptr<Renderer> m_GraphicsContext;
	};
}

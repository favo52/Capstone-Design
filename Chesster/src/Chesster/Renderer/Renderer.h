#pragma once

#include "Chesster/Renderer/Texture.h"

#include <glm/glm.hpp>

struct SDL_Window;
struct SDL_Renderer;

namespace Chesster
{
	/*	Represents a rectangle/square's left, right, bottom and top sides. */
	struct RectBounds
	{
		float left{ 0.0f }, right{ 0.0f };
		float bottom{ 0.0f }, top{ 0.0f };

		RectBounds operator+(const float& value) const;
	};

	/*	Abstraction of the SDL Renderer.
		The Renderer class draws rectangles and textures to the screen.
		It also changes the viewport size, sets clear color and clears the screen. */
	class Renderer
	{
	public:
		/** Renderer Constructor.
			Creates a 2D renderering context for the given window.
		 @param windowHandle The window where rendering is displayed. */
		Renderer(SDL_Window* windowHandle);

		/*	Destroys the SDL renderer. */
		virtual ~Renderer();

		/** Draws a filled rectangle with the specified dimensions and color.
		 @param rect The dimensions of the rectangle.
		 @param color The color of the rectangle. */
		static void DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color);

		/** Draws a fixed texture to the render target.
		 @param texture The texture to be drawn. */
		static void DrawTexture(const Texture* texture);
		
		/** Updates the viewport dimensions. Must be called after a window resize event.
		 @param x The new top left x position.
		 @param y The new top left y position.
		 @param width The new viewport width.
		 @param height The new viewport height. */
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		/** Set the color used for drawing operations or filling rectangles.
		 @param color The color to be used. */
		static void SetClearColor(const glm::vec4& color);
		
		/*	Clear the current rendering target with the drawing color. */
		static void Clear();
		
		/** Is is used to retrieve the SDL_Renderer.
		 @return A pointer to the Renderer's SDL_Renderer member variable. */
		static SDL_Renderer* Get() { return s_Renderer; }

	private:
		static SDL_Renderer* s_Renderer;	// A structure representing rendering state.
	};
}

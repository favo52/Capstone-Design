#pragma once

#include "Chesster/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Chesster
{
	/// <summary>
	/// Utility for storing the dimensions of all four sides of a square.
	/// </summary>
	struct QuadBounds
	{
		float left{ 0.0f }, right{ 0.0f };
		float bottom{ 0.0f }, top{ 0.0f };

		QuadBounds operator+(const float& value) const;
	};

	/// <summary>
	/// The Renderer class draws rectangles and textures to the screen.
	/// There's no need to create a Renderer object, as all functions are static.
	/// </summary>
	class Renderer
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="rect">The dimensions of the rectangle.</param>
		/// <param name="color">The color of the rectangle.</param>
		static void DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color);

		/// <summary>
		/// Draws a fixed texture to the render target.
		/// </summary>
		/// <param name="texture">The texture to be drawn.</param>
		static void DrawTexture(const Texture* texture);

		/// <summary>
		/// Draws a fixed texture to the render target with optional rotation and texture clipping.
		/// </summary>
		/// <param name="texture">The texture to be drawn.</param>
		static void DrawTextureEx(const Texture* texture);

		/// <summary>
		/// Updates the viewport size. Must be called after a window resize event.
		/// </summary>
		/// <param name="width">The new window width.</param>
		/// <param name="height">The new window height.</param>
		static void OnWindowResize(uint32_t width, uint32_t height);
	};
}

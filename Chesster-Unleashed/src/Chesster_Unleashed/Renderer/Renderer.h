#pragma once

#include "Chesster_Unleashed/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Chesster
{
	struct QuadBounds
	{
		float left{ 0.0f }, right{ 0.0f };
		float bottom{ 0.0f }, top{ 0.0f };

		QuadBounds operator+(const float& value) const;
	};

	class Renderer
	{
	public:
		static void DrawRect(const SDL_Rect& rect, const glm::vec4& color);
		static void DrawFilledRect(const SDL_Rect& rect, const glm::vec4& color);
		static void DrawLine(const SDL_Point& x, const SDL_Point& y, const glm::vec4& color);
		static void DrawTexture(const Texture* texture);
		static void DrawTextureEx(const Texture* texture);

		static void OnWindowResize(uint32_t width, uint32_t height);
	};
}

#pragma once

#include "Chesster_Unleashed/Core/Layer.h"
#include "Chesster_Unleashed/Core/Window.h"
#include "Chesster_Unleashed/Renderer/Framebuffer.h"
#include "Chesster_Unleashed/Game/Board.h"

#include "Chesster_Unleashed/ImGui/Panels/ConsolePanel.h"

#include <glm/glm.hpp>

namespace Chesster
{
	class GameLayer : public Layer
	{
	public:
		GameLayer();
		virtual ~GameLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event & sdlEvent) override;
		virtual void OnUpdate(const std::chrono::duration<double>&dt) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

	private:
		void UpdateSquareColors();

		bool IsPointInQuad(const glm::vec2& point, const QuadBounds& quad);

	private:
		Window& m_Window;
		std::shared_ptr<Framebuffer> m_Framebuffer;

		bool m_ViewportFocused{ false }, m_ViewportHovered{ false };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		Board m_Board;

		glm::vec2 m_MouseCoords{ 0.0f, 0.0f };
		glm::vec2 m_ViewportMousePos{ 0.0f, 0.0f };
		uint32_t m_MouseButton{ 0 };

		// Panels
		static ConsolePanel m_ConsolePanel;

		// Color settings
		glm::vec4 ClearColor = { 0.141f * 255.0f, 0.203f * 255.0f, 0.270f * 255.0f, 1.0f * 255.0f };
		//glm::vec4 SquareColor1 = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
		glm::vec4 SquareColor1 = { 0.084f, 0.342f, 0.517f, 1.0f }; // Blue
		glm::vec4 SquareColor2 = { 1.0f, 1.0f, 1.0f, 1.0f }; // White
	};
}

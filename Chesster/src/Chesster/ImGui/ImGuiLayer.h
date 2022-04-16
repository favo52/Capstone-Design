#pragma once

#include "Chesster/Core/Layer.h"

namespace Chesster
{
	/*	Contains the necesary backend code for drawing the ImGui GUI. */
	class ImGuiLayer : public Layer
	{
	public:
		/*	Default destructor. */
		virtual ~ImGuiLayer() = default;

		/*	Configures ImGui context, style, fonts, and rendering backends. */
		virtual void OnAttach() override;

		/*	Destroys the ImGui implementation and performs cleanup. */
		virtual void OnDetach() override;

		/*	Starts the Dear ImGui frame. Must be called before attempting to draw anything. */
		void Begin();

		/*	Performs the ImGui rendering. */
		void End();

	private:
		/*	Changes the colors of the ImGui style to a darker theme. */
		void SetDarkTheme();
	};
}

#pragma once

#include "Chesster/Core/Layer.h"

namespace Chesster
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

	private:
		void SetDarkTheme();
	};
}

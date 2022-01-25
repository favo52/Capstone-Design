#pragma once

#include "Chesster_Unleashed/Core/Layer.h"

namespace Chesster
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

		bool IsEventsBlocked() { return m_BlockEvents; }
		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkTheme();

	private:
		bool m_BlockEvents{ true };
	};
}

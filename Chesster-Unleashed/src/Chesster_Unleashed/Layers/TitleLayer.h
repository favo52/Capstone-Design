#pragma once

#include "Chesster_Unleashed/Core/Layer.h"

namespace Chesster
{
	class TitleLayer : public Layer
	{
	public:
		TitleLayer();
		virtual ~TitleLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(SDL_Event& sdlEvent) override;
		virtual void OnUpdate(const std::chrono::duration<double>& dt) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;

	private:

	};
}

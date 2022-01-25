#include "pch.h"
#include "Chesster_Unleashed/Core/LayerStack.h"

namespace Chesster
{
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		// Push overlays into the back
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto itr = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (itr != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(itr);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto itr = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (itr != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(itr);
		}
	}
}

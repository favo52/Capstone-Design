#pragma once

#include "Chesster/Core/Layer.h"

namespace Chesster
{
	/*	Holds and manages all the Layer objects used in the Application. */
	class LayerStack
	{
	public:
		/*	Default constructor. */
		LayerStack() = default;

		/*	Empties the layer stack and releases the memory. */
		~LayerStack();
		
		/** Inserts a Layer to the layer stack.
		 @param layer A pointer to the Layer to be inserted. */
		void PushLayer(Layer* layer);

		/** Inserts a Layer to the back of the layer stack.
		 @param overlay A pointer to the Layer to be inserted. */
		void PushOverlay(Layer* overlay);

		/** Removes the Layer from the layer stack.
		 @param layer A pointer to the Layer to be removed. */
		void PopLayer(Layer* layer);

		/** Removes the Layer from the back of the layer stack.
		 @param overlay A pointer to the Layer to be removed. */
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

		std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex{ 0 };
	};
}

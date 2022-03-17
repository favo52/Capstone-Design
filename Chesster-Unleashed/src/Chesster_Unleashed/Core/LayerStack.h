#pragma once

#include "Chesster_Unleashed/Core/Layer.h"

namespace Chesster
{
	/// <summary>
	/// Holds and manages all the Layer objects used in the Application.
	/// </summary>
	class LayerStack
	{
	public:
		/// <summary>
		/// Default constructor.
		/// </summary>
		LayerStack() = default;

		/// <summary>
		/// Empties the layer stack and releases the memory.
		/// </summary>
		~LayerStack();
		
		/// <summary>
		/// Inserts a Layer to the layer stack.
		/// </summary>
		/// <param name="layer">A pointer to the Layer to be inserted.</param>
		void PushLayer(Layer* layer);

		/// <summary>
		/// Inserts a Layer to the back of the layer stack.
		/// </summary>
		/// <param name="overlay">A pointer to the Layer to be inserted.</param>
		void PushOverlay(Layer* overlay);

		/// <summary>
		/// Removes the Layer from the layer stack.
		/// </summary>
		/// <param name="layer">A pointer to the Layer to be removed.</param>
		void PopLayer(Layer* layer);

		/// <summary>
		/// Removes the Layer from the back of the layer stack.
		/// </summary>
		/// <param name="overlay">A pointer to the Layer to be removed.</param>
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

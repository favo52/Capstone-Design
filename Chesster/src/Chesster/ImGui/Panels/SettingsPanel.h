#pragma once

#include <glm/glm.hpp>

namespace Chesster
{
	/*	The settings panel holds all the buttons to */
	class SettingsPanel
	{
	public:
		SettingsPanel();

		/*	Checks if a button has been pressed and performs the
			corresponding action if done so. */
		void OnUpdate();

		/*	Draws the Settings panel to the screen. */
		void OnImGuiRender();

		/*	Sets the robot connection toggle to true or false.
		 @param toggle The new true or false state. */
		void SetRobotConnection(bool toggle) { m_IsRobotConnected = toggle; }

		/**	Retrieves the Clear Color from the Settings Panel.
		 @return A glm::vec4 of the Clear Color. */
		static const glm::vec4& ClearColor() { return s_ClearColor; }

	private:
		/*	Changes the square colors when it is changed during runtime. */
		void UpdateSquareColors();

	private:
		int m_SkillLevel;	// Defaults to 0 (Minimum)
		int m_ELORating;	// Defaults to 1350 (Minimum)

		// Difficulty toggles
		bool m_IsNewSkillLevel;
		bool m_IsNewELO;
		bool m_IsToggleELOPressed;
		bool m_IsELOActive;

		// Camera toggles
		bool m_IsCameraButtonPressed;
		bool m_IsCameraConnected;

		// Robot toggles
		bool m_IsRobotButtonPressed;
		bool m_IsRobotConnected;

		// Board Color settings
		static glm::vec4 s_ClearColor;		// Background/Screen refresh color
		static glm::vec4 s_SquareColor1;
		static glm::vec4 s_SquareColor2;
	};
}

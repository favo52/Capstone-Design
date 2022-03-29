#pragma once

#include <glm/glm.hpp>

namespace Chesster
{
	/*	The settings panel holds all the buttons to */
	class SettingsPanel
	{
	public:
		/*	Draws the Settings panel to the screen. */
		static void OnImGuiRender();

		static glm::vec4 s_ClearColor;

		static int SkillLevel;
		static int ELO;

		// Difficulty toggles
		static bool IsNewSkillLevel;
		static bool IsNewELO;
		static bool IsToggleELO;
		static bool IsELOActive;

		// Camera toggles
		static bool IsCameraButtonPressed;
		static bool IsCameraConnected;

		// Robot toggles
		static bool IsRobotButtonPressed;
		static bool IsRobotConnected;

	private:
		static void UpdateSquareColors();

		// Color settings
		static glm::vec4 s_SquareColor1;
		static glm::vec4 s_SquareColor2;
	};
}

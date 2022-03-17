#pragma once

#include <glm/glm.hpp>

namespace Chesster
{
	/// <summary>
	/// The settings panel holds all the buttons to
	/// </summary>
	class SettingsPanel
	{
	public:
		SettingsPanel() = default;
		virtual ~SettingsPanel() = default;

		void OnImGuiRender();

	public:
		static glm::vec4 ClearColor;

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
		void UpdateSquareColors();

	private:
		// Color settings
		glm::vec4 SquareColor1 = { 0.084f, 0.342f, 0.517f, 1.0f }; // Blueish
		glm::vec4 SquareColor2 = { 1.0f, 1.0f, 1.0f, 1.0f }; // White
	};
}

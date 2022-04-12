#pragma once

#include <glm/glm.hpp>

namespace Chesster
{
	/*	The settings panel draws and manages all the buttons and sliders to
		control the connections with the camera, robot and the chess engine. */
	class SettingsPanel
	{
	public:
		/*	Default constructor. Sets all the member variables default values. */
		SettingsPanel();

		/*	Draws the Settings panel to the screen. */
		void OnImGuiRender();

		/**	Sets the camera button connection status to Connect or Disconnect.
		 @param toggle The new true or false state. */
		void SetCameraButtonStatus(bool toggle) { m_IsCameraConnected = toggle; }

		/**	Sets the robot button connection status to Connect or Disconnect.
		 @param toggle The new true or false state. */
		void SetRobotButtonStatus(bool toggle) { m_IsRobotConnected = toggle; }

		/**	Retrieves the Clear Color from the Settings Panel.
		 @return A glm::vec4 of the Clear Color. */
		const glm::vec4& GetClearColor() { return m_ClearColor; }

	private:
		/*	This function is called when the button under the Cognex section is pressed.
			It will start the camera connection threads if there is no connection, or it
			will disconnect the camera if a connection already exists. */
		void OnCameraButtonPressed();

		/*	This function is called when the button under the Staubli section is pressed.
			It will start the chesster server connection thread if there is no connection,
			or it will shut down the server if a connection already exists. */
		void OnRobotButtonPressed();

		/*	Updates the square colors when it is changed during runtime. */
		void OnNewSquareColor();

	private:
		std::string m_CameraIP, m_CameraTelnetPort, m_CameraTCPDevicePort,
					m_RobotIP, m_RobotPort;

		int m_SkillLevel;			// Defaults to 0 (Minimum)
		int m_ELORating;			// Defaults to 1350 (Minimum)

		bool m_IsCameraConnected;	// Keeps track of the camera connection
		bool m_IsRobotConnected;	// Keeps track of the robot connection

		glm::vec4 m_ClearColor;		// Background/Screen refresh color
		glm::vec4 m_SquareColor1;	// Color of all odd squares
		glm::vec4 m_SquareColor2;	// Color of all even squares

		friend class Network;		// Gives Network access to the IPs and Ports
	};
}

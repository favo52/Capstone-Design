/*
* Copyright 2022-present, Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

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
					m_ServerIP, m_ServerPort;

		int m_SkillLevel;			// 0 to 20 (defaults to 0)
		int m_ELORating;			// 1350 to 2850 (defaults to 1350)
		int m_MultiPV;				// 1 to 500 (defaults to 1)

		bool m_IsCameraConnected;	// Keeps track of the camera connection
		bool m_IsRobotConnected;	// Keeps track of the robot connection

		glm::vec4 m_ClearColor;		// Background/Screen refresh color
		glm::vec4 m_SquareColor1;	// Color of all odd squares
		glm::vec4 m_SquareColor2;	// Color of all even squares

		friend class Network;		// Gives Network access to the IPs and Ports
	};
}

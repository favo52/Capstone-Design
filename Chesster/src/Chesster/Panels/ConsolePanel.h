/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
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

namespace Chesster
{
	/* The ConsolePanel displays all the messages related to the Chess Engine.
	   It also contains several utility buttons to reset the board or fix the camera data. */
	class ConsolePanel
	{
	public:
		ConsolePanel();

		/* Draws the Console Panel ImGui window and all its components. */
		void OnImGuiRender();

		/* Adds a message to the text area of the Console Panel. */
		void AddLog(const std::string& msg);
		
		/* Clears the text logging area. */
		void ClearLog() { m_Items.clear(); }

	private:
		/* The next text will be changed to this Font.
		 @param index The array index of the ImGui Font. */
		void PushFont(int index);

	private:
		std::vector<std::string> m_Items;
		
		bool m_ScrollToBottom;
	};
}

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

#include <imgui_internal.h>

namespace Chesster
{
	/* The LogPanel is used to print status updates of the TCP/IP communication. */
	class LogPanel
	{
	public:
		LogPanel() = default;

		/* Clears the log. */
		virtual ~LogPanel() { Clear(); }

		/* Draws the Log Panel ImGui window. */
		void OnImGuiRender();

		/* Appends a message to the ImGuiTextBuffer. It is displayed in the Log Panel.
		 @param message The text to be displayed. */
		void AddLog(std::string message);

		/* Clears the ImGuiTextBuffer. */
		void Clear() { m_Log.clear(); }

	private:
		ImGuiTextBuffer m_Log;
	};
}

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

#include <imgui_internal.h>

namespace Chesster
{
	class LogPanel
	{
	public:
		LogPanel() = default;

		void OnImGuiRender();

		void AddLog(std::string message);
		void Clear();

	private:
		ImGuiTextBuffer m_Log;
		//std::array<char, 64> m_Buffer;
	};
}

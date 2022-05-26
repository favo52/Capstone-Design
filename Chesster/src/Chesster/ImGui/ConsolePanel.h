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

namespace Chesster
{
	class ConsolePanel
	{
	public:
		ConsolePanel();

		void OnImGuiRender();

		void AddLog(const std::string& msg);
		void ClearLog() { m_Items.clear(); }

	private:
		void PushFont(int index);

	private:
		std::vector<std::string> m_Items;
		
		bool m_ScrollToBottom;
	};
}

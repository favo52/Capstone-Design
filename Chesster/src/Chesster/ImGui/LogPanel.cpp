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

#include "pch.h"
#include "Chesster/ImGui/LogPanel.h"

#include <imgui.h>

namespace Chesster
{
	LogPanel::~LogPanel()
	{
		Clear();
	}

	void LogPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Communication Log");

		if (ImGui::Button("Clear")) { m_Log.clear(); }

		ImGui::Separator();
		ImGui::BeginChild("log");

		ImGui::TextUnformatted(m_Log.begin(), m_Log.end());

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}

	void LogPanel::AddLog(std::string message)
	{
		message += "\n";
		m_Log.appendf(message.c_str());
	}

	void LogPanel::Clear()
	{
		m_Log.clear();
	}
}

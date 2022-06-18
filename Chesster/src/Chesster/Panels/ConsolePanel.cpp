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
#include "Chesster/Panels/ConsolePanel.h"

#include "Chesster/Layers/GameLayer.h"

#include <imgui.h>

namespace Chesster
{
	ConsolePanel::ConsolePanel() :
		m_ScrollToBottom{ true }
	{
		AddLog("Welcome to Chesster!");
	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Chess Engine Console");

		ImGui::TextWrapped("This console is used to interact with the Chess Engine.");

		if (ImGui::IsWindowFocused())
		{
			m_ScrollToBottom = true;
			ImGui::SetNextWindowFocus();
		}

		// Small buttons at top
		if (ImGui::SmallButton("Clear")) m_Items.clear();
		ImGui::SameLine();
		bool CopyToClipboard = ImGui::SmallButton("Copy");
		ImGui::Separator();

		// Filter
		ImGuiTextFilter Filter;
		ImGui::SameLine();
		Filter.Draw("Filter (ex. \"d2d4\", \"error\")", 180);
		ImGui::Separator();

		// Reserve enough left-over height
		const float FooterHeightToReserve = (ImGui::GetStyle().ItemSpacing.y * 3) + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -FooterHeightToReserve - 20), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		PushFont(1);

		// Print text to console
		if (CopyToClipboard) ImGui::LogToClipboard();
		for (const auto& item : m_Items)
		{
			if (!Filter.PassFilter(item.c_str()))
				continue;

			ImGui::TextUnformatted(item.c_str());
		}
		if (CopyToClipboard) ImGui::LogFinish();

		if (m_ScrollToBottom || ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
		m_ScrollToBottom = false;

		ImGui::PopFont();
		ImGui::PopStyleVar();
		ImGui::EndChild();	// End "ScrollingRegion"

		// Reset and Evaluate buttons
		ImGui::Separator();
		PushFont(2);
		if (ImGui::Button("Reset Game", ImVec2(100, 50)))
			GameLayer::Get().ResetGame();

		ImGui::SameLine();
		if (ImGui::Button("Evaluate\n   Game", ImVec2(100, 50)))
			GameLayer::Get().GetChessEngine().EvaluateGame();

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		ImGui::BeginDisabled(!GameLayer::Get().GetSettingsPanel().IsCameraConnected());

		if (ImGui::Button("Synchronize\n      Image", { 100, 50 }))
		{
			GameLayer::Get().ArmIsSettled();
			GameLayer::Get().GetNetwork().SendToCamera("SE8\r\n");
		}

		ImGui::EndDisabled();

		ImGui::PopFont();
		ImGui::Separator();

		ImGui::End(); // End "Chess Engine Console"
	}

	void ConsolePanel::AddLog(const std::string& msg)
	{
		m_Items.emplace_back(msg);
		m_ScrollToBottom = true;
	}

	void ConsolePanel::PushFont(int index)
	{
		ImFont* font = ImGui::GetIO().Fonts->Fonts[index];
		ImGui::PushFont(font);
	}
}

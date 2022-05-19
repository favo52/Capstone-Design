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

#include "pch.h"
#include "Chesster/ImGui/ConsolePanel.h"

#include "Chesster/Layers/GameLayer.h"
#include "Chesster/Connections/ChessEngine.h"

#include <imgui.h>

namespace Chesster
{
	ConsolePanel::ConsolePanel() :
		m_ScrollToBottom{ true }
	{
		m_Commands.reserve(2);
		m_Commands.emplace_back("help");
		m_Commands.emplace_back("uci");
		m_Commands.emplace_back("isready");
		m_Commands.emplace_back("setoption name");
		AddLog("Welcome to Chesster!");
	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Chess Engine Console");

		ImGui::TextWrapped("This console is used to interact with the Chess Engine.");
		ImGui::TextWrapped("Enter 'HELP' to see a list of commands.");

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

		// Reserve enough left-over height for 1 separator + 1 input text
		const float FooterHeightToReserve = (ImGui::GetStyle().ItemSpacing.y * 3) + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -FooterHeightToReserve - 54), false, ImGuiWindowFlags_HorizontalScrollbar);

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
		
		ImGui::Separator();

		// Command-line Input
		bool reclaimFocus{ false };
		std::array<char, 128> InputBuffer = {};
		if (ImGui::InputText("Input", InputBuffer.data(), InputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (!InputBuffer.empty())
				ExecCommand(std::string(InputBuffer.data()));

			reclaimFocus = true;
		}

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaimFocus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		// Reset and Evaluate buttons
		ImGui::Separator();
		PushFont(2);
		if (ImGui::Button("Reset Game", ImVec2(100, 50)))
			GameLayer::Get().ResetGame();

		ImGui::SameLine();
		if (ImGui::Button("Evaluate", ImVec2(100, 50)))
			GameLayer::Get().GetChessEngine().EvaluateGame();

		ImGui::SameLine();
		ImGui::BeginDisabled(!GameLayer::Get().GetSettingsPanel().IsCameraConnected());

		if (ImGui::Button("\t\tEnd\nHuman Turn", { 100, 50 }))
		{
			GameLayer::Get().EndPlayerTurn();
			GameLayer::Get().GetNetwork().SendToCamera("SE8\r\n");
		}

		ImGui::SameLine();
		if (ImGui::Button("Fix Board", { 100, 50 }))
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

	void ConsolePanel::ExecCommand(std::string& command)
	{
		AddLog("# " + command + "\n");

		for (char& ch : command)
			ch = tolower(ch);

		// Process commands
		if (command == "help")
		{
			AddLog("\nCommands:");
			for (int i = 0; i < m_Commands.size(); i++)
				AddLog("- " + m_Commands[i]);

			m_ScrollToBottom = true;
			return;
		}

		ChessEngine& chessEngine = GameLayer::Get().GetChessEngine();

		command += '\n';
		chessEngine.WriteToEngine(command);

		if (command != "quit\n" || command != "ucinewgame\n")
		{
			std::string test;
			std::istringstream iss{ command };
			iss >> test;

			if (test != "position\n");
			{
				//std::string engineReply = chessEngine.ReadFromEngine();
				//GameLayer::Get().GetConsolePanel().AddLog("\n" + engineReply);
			}
		}

		m_ScrollToBottom = true;
	}
}

#include "pch.h"
#include "Chesster/ImGui/Panels/ConsolePanel.h"

#include "Chesster/Layers/GameLayer.h"

namespace Chesster
{
	ConsolePanel::ConsolePanel() :
		m_ScrollToBottom{ false }
	{
		m_Commands.push_back("HELP");
		AddLog("Welcome to Chesster!");
	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Chess Engine Console");

		ImGui::TextWrapped("This console is used to interact with the Chess Engine.");
		ImGui::TextWrapped("Enter 'HELP' for help.");

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
		std::array<char, 256> InputBuffer = {};
		if (ImGui::InputText("Input", InputBuffer.data(), InputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			std::string InputStr{ InputBuffer.data() };
			if (!InputStr.empty())
				ExecCommand(InputStr);

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
			GameLayer::Get().GetConnector()->EvaluateGame();

		ImGui::PopFont();
		ImGui::Separator();

		ImGui::End(); // End "Chess Engine Console"
	}

	void ConsolePanel::AddLog(const std::string& msg)
	{
		m_Items.push_back(msg);
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
			ch = toupper(ch);

		// Process commands
		if (command == "HELP")
		{
			AddLog("Commands:");
			for (int i = 0; i < m_Commands.size(); i++)
				AddLog("- " + m_Commands[i]);
		}

		m_ScrollToBottom = true;
	}
}

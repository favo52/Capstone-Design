#pragma once

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

namespace Chesster
{
	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	struct AppLogGUI
	{
		ImGuiTextBuffer Buf;
		ImVector<int>	LineOffsets;
		bool			AutoScroll; // Keep scrolling if already at the bottom.

		AppLogGUI() :
			AutoScroll{ true }
		{
			Clear();
		}

		void Clear()
		{
			Buf.clear();
			LineOffsets.clear();
			LineOffsets.push_back(0);
		}

		void AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendfv(fmt, args);
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void Draw(const char* title, bool* p_open, ImGuiWindowFlags flags)
		{
			// Set ImGui window position and size
			ImGui::SetNextWindowPos(ImVec2(720, 0));
			ImGui::SetNextWindowSize(ImVec2(560, 520), ImGuiCond_FirstUseEver);
			if (!ImGui::Begin(title, p_open, flags))
			{
				ImGui::End();
				return;
			}

			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox("Auto-scroll", &AutoScroll);
				ImGui::EndPopup();
			}

			// Main window
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");

			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();

			// Print everything to the ImGui window
			ImGui::TextUnformatted(buf, buf_end);

			ImGui::PopStyleVar();

			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
			ImGui::End();
		}
	};

	struct AppSettingsGUI
	{
		bool m_ResetBoard;
		bool m_EvaluateBoard;
		bool m_ChangeDifficulty;

		int m_OldDiff;
		static int m_Difficulty;

		AppSettingsGUI() :
			m_ResetBoard{ false },
			m_EvaluateBoard{ false },
			m_ChangeDifficulty{ false },
			m_OldDiff{ 0 }
		{
		}

		void Draw(const char* title, bool* p_open, ImGuiWindowFlags flags)
		{
			// Set ImGui window position and size
			ImGui::SetNextWindowPos(ImVec2(720, 520));
			ImGui::SetNextWindowSize(ImVec2(560, 200), ImGuiCond_FirstUseEver);
			if (!ImGui::Begin(title, p_open, flags))
			{
				ImGui::End();
				return;
			}

			if (ImGui::BeginPopup("Difficulty"))
			{
				ImGui::SliderInt("Difficulty", &m_Difficulty, 0, 20);
				ImGui::SameLine(); HelpMarker("CTRL+click to input value.");
				if (m_OldDiff != m_Difficulty)
					m_ChangeDifficulty = true;
				m_OldDiff = m_Difficulty;
				ImGui::EndPopup();
			}

			// Buttons
			if (ImGui::Button("Reset Game", ImVec2(100, 50)))
				m_ResetBoard = true;
			ImGui::SameLine();
			if (ImGui::Button("Evaluatate\nPosition", ImVec2(100, 50)))
				m_EvaluateBoard = true;
			ImGui::SameLine();
			if (ImGui::Button("Change\nDifficulty", ImVec2(100, 50)))
				ImGui::OpenPopup("Difficulty");

			// Info area
			ImGui::Separator();
			ImGui::BeginChild("Information");
			ImGui::SetWindowFontScale(1.1);
			ImGui::Text
			(
				"CONTROLS:\n"
				"[Mouse] Drag and drop pieces.\n"
				"[Spacebar] Computer moves.\n"
				"[Escape] Open pause menu.\n"
			);

			ImGui::Spacing();
			ImGui::Text
			(
				"Mouse coordinates (x=%.1f, y=%.1f)",
				ImGui::GetIO().MousePos.x,
				ImGui::GetIO().MousePos.y
			);

			ImGui::Spacing();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::EndChild();
			ImGui::End();
		}
	};
}

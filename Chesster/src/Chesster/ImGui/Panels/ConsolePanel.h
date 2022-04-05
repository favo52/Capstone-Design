#pragma once

#include <imgui.h>

namespace Chesster
{
	struct ConsoleButtons
	{
		static bool ResetBoardButton;
		static bool EvaluateBoardButton;

		void Draw();
	};

	class ConsolePanel
	{
	public:
		ConsolePanel();
		virtual ~ConsolePanel();

		void OnImGuiRender();

		void AddLog(const char* fmt, ...); IM_FMTARGS(2);
		void AddLog(const std::string& msg);

	private:
		void ClearLog();

		void PushFont(int index);
		void PopFont();

		void ExecCommand(const char* command_line);

	private:
		std::array<char, 256> InputBuffer;

		ImVector<char*> Items;
		std::vector<std::string> myItems;
		ImVector<const char*> Commands;
		ImVector<char*> History;
		int HistoryPos{ -1 };

		ImGuiTextFilter Filter;
		
		bool AutoScroll;
		bool ScrollToBottom;

		ConsoleButtons m_Buttons;
	};
}

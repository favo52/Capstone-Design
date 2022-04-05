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

		void ClearLog();
		void AddLog(const char* fmt, ...); IM_FMTARGS(2);

		void OnImGuiRender();

	private:
		void PushFont(int index);
		void PopFont();

		void ExecCommand(const char* command_line);

	private:
		std::array<char, 256> InputBuffer;

		ImVector<char*> Items;
		ImVector<const char*> Commands;
		ImVector<char*> History;
		int HistoryPos{ -1 };

		ImGuiTextFilter Filter;
		
		bool AutoScroll;
		bool ScrollToBottom;

		ConsoleButtons m_Buttons;
	};
}

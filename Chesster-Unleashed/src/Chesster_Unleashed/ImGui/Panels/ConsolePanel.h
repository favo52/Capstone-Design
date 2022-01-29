#pragma once

#include <imgui.h>

namespace Chesster
{
	struct ConsoleButtons
	{
		bool ResetBoardButton{ false };
		bool EvaluateBoardButton{ false };

		void Draw();
	};

	class ConsolePanel
	{
	public:
		ConsolePanel();
		virtual ~ConsolePanel();

		// Portable helpers
		static int Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
		static int Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
		static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
		static void Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

		void ClearLog();
		void AddLog(const char* fmt, ...); IM_FMTARGS(2);

		void OnImGuiRender(const char* title, bool* p_open = nullptr);

		void PushFont(int index);
		void PopFont();

		void ExecCommand(const char* command_line);

		// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
		static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
		{
			ConsolePanel* console = (ConsolePanel*)data->UserData;
			return console->TextEditCallback(data);
		}

		int	 TextEditCallback(ImGuiInputTextCallbackData* data);

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

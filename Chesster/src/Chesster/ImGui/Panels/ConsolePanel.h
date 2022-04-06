#pragma once

#include <imgui.h>

namespace Chesster
{
	class ConsolePanel
	{
	public:
		ConsolePanel();
		virtual ~ConsolePanel() = default;

		void OnImGuiRender();

		void AddLog(const std::string& msg);

	private:
		void PushFont(int index);
		void ExecCommand(std::string& command);

	private:
		std::vector<std::string> m_Items;
		std::vector<std::string> m_Commands;
		
		bool m_ScrollToBottom;
	};
}

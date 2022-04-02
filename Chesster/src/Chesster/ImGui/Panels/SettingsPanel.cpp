#include "pch.h"
#include "Chesster/ImGui/Panels/SettingsPanel.h"

#include "Chesster/Connections/TCPConnection.h"
#include "Chesster/Game/Board.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Chesster
{
	glm::vec4 SettingsPanel::s_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black

	int SettingsPanel::SkillLevel{ 0 };
	int SettingsPanel::ELO{ 1350 };

	bool SettingsPanel::IsNewSkillLevel{ false };
	bool SettingsPanel::IsNewELO{ false };
	bool SettingsPanel::IsToggleELO{ false };
	bool SettingsPanel::IsELOActive{ false };

	bool SettingsPanel::IsCameraButtonPressed{ false };
	bool SettingsPanel::IsCameraConnected{ false };

	bool SettingsPanel::IsRobotButtonPressed{ false };
	bool SettingsPanel::IsRobotConnected{ false };

	glm::vec4 SettingsPanel::s_SquareColor1 = { 0.084f, 0.342f, 0.517f, 1.0f };	// Blueish
	glm::vec4 SettingsPanel::s_SquareColor2 = { 1.0f, 1.0f, 1.0f, 1.0f };		// White

	static void DrawIntControl(const std::string& label, int& value, bool& button, int min, int max, float columnWidth = 100.0f)
	{
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });

		if (ImGui::SliderInt("##Diff", &value, min, max))
			button = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	static bool DrawColorEdit4Control(const std::string& label, glm::vec4& values, float columnWidth = 100.0f)
	{
		bool update{ false };

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(200.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
		if (ImGui::ColorEdit4("##RGB", glm::value_ptr(values)))
			update = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		
		return update;
	}

	static bool DrawInputText(const std::string& label, std::array<char, 64>& buffer, const std::string& text, float columnWidth = 100.0f)
	{
		bool update{ false };

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(110.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });

		std::strncpy(buffer.data(), text.c_str(), buffer.size());
		if (ImGui::InputText("##Text", buffer.data(), buffer.size()))
			update = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return update;
	}

	template<typename T, typename UIFunction>
	static void DrawSection(const std::string& name, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();

		if (open)
		{
			uiFunction();
			ImGui::TreePop();
		}
	}

    void SettingsPanel::OnImGuiRender()
    {
		ImGui::Begin("Settings");

		TCPConnection& TCP = TCPConnection::Get();
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[2];

		class Cognex {};
		DrawSection<Cognex>("Cognex Camera", [&]()
		{
			ImGui::PushFont(boldFont); 
			const char* buttonText = (!IsCameraConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(buttonText, { 100, 50 }))
				IsCameraButtonPressed = true;
			ImGui::PopFont();

			ImGui::Separator();
			std::array<char, 64> ip_buffer = {};
			if (DrawInputText("IP Address", ip_buffer, TCP.s_CameraIP, 120.0f))
				TCP.s_CameraIP = std::string(ip_buffer.data());

			ImGui::SameLine();
			ImGui::Text("localhost");

			std::array<char, 64> commandPort_buffer = {};
			if (DrawInputText("Command Port", commandPort_buffer, TCP.s_CameraCommandPort, 120.0f))
				TCP.s_CameraCommandPort = std::string(commandPort_buffer.data());

			ImGui::SameLine();
			ImGui::Text("23");

			std::array<char, 64> streamPort_buffer = {};
			if (DrawInputText("Stream Port", streamPort_buffer, TCP.s_CameraStreamPort, 120.0f))
				TCP.s_CameraStreamPort = std::string(streamPort_buffer.data());

			ImGui::SameLine();
			ImGui::Text("3000");
		});

		class Staubli {};
		DrawSection<Staubli>("Staubli Robotic Arm", [&]()
		{
			ImGui::PushFont(boldFont);
			const char* buttonText = (!IsRobotConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(buttonText, { 100, 50 }))
				IsRobotButtonPressed = true;
			ImGui::PopFont();

			ImGui::Separator();
			std::array<char, 64> ip_buffer = {};
			if (DrawInputText("IP Address", ip_buffer, TCP.s_RobotIP, 100.0f))
				TCP.s_RobotIP = std::string(ip_buffer.data());

			ImGui::SameLine();
			ImGui::Text("192.168.7.10");

			std::array<char, 64> port_buffer = {};
			if (DrawInputText("Port", port_buffer, TCP.s_RobotPort, 100.0f))
				TCP.s_RobotPort = std::string(port_buffer.data());

			ImGui::SameLine();
			ImGui::Text("15000");
		});

		class DifficultySlider {};
		DrawSection<DifficultySlider>("Engine Difficulty", []()
		{
			DrawIntControl("Skill Level", SkillLevel, IsNewSkillLevel, 0, 20);
			DrawIntControl("ELO Rating", ELO, IsNewELO, 1350, 2850);

			if (ImGui::Checkbox("Activate ELO (Overrides Skill Level)", &IsELOActive))
				IsToggleELO = true;
		});

		static glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		DrawSection<glm::vec4>("Colors", [&]()
		{
			if (DrawColorEdit4Control("Border", clearColor, 60.0f))
				s_ClearColor = clearColor * 255.0f;

			if (DrawColorEdit4Control("Odds", s_SquareColor1, 60.0f))
				UpdateSquareColors();

			if (DrawColorEdit4Control("Evens", s_SquareColor2, 60.0f))
				UpdateSquareColors();
		});

		// Display the framerate
		ImGui::Separator();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();

		ImGui::End(); // End "Settings"
    }

	void SettingsPanel::UpdateSquareColors()
	{
		for (Board::Square& square : Board::GetBoardSquares())
		{
			const glm::vec4 newColor = ((square.Index + 1) % 2 == 0) ? s_SquareColor1 : s_SquareColor2;
			square.Color = newColor * 255.0f;
		}
	}
}

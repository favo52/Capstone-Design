#include "pch.h"
#include "Chesster/ImGui/Panels/SettingsPanel.h"

#include "Chesster/Layers/GameLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Chesster
{	
	SettingsPanel::SettingsPanel() :
		m_CameraIP{ "localhost" },
		m_CameraTelnetPort{ "23" },
		m_CameraTCPDevicePort{ "3000" },
		m_RobotIP{ "192.168.7.10" },
		m_RobotPort{ "15000" },
		m_SkillLevel{ 0 },
		m_ELORating{ 1350 },
		m_IsCameraConnected{ false },
		m_IsRobotConnected{ false },
		m_ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f },			// Black
		m_SquareColor1{ 0.084f, 0.342f, 0.517f, 1.0f },	// Blueish
		m_SquareColor2{ 1.0f, 1.0f, 1.0f, 1.0f }		// White
	{
	}

	static bool DrawIntSliderControl(const std::string& label, int& value, int min, int max, float columnWidth = 100.0f)
	{
		bool update{ false };

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });

		if (ImGui::SliderInt("##Diff", &value, min, max))
			update = true;

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return update;
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

		Network& network = Network::Get();
		auto boldFont = ImGui::GetIO().Fonts->Fonts[2];

		class Cognex {};
		DrawSection<Cognex>("Cognex Camera", [&]()
		{
			ImGui::PushFont(boldFont);
			const char* buttonText = (!m_IsCameraConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(buttonText, { 100, 50 }))
				OnCameraButtonPressed();

			ImGui::SameLine();
			ImGui::BeginDisabled(!m_IsCameraConnected);

			if (ImGui::Button("Take Picture", { 100, 50 }))
				Network::Get().SendToCamera("SE8\n");

			ImGui::EndDisabled();
			ImGui::PopFont();

			ImGui::Separator();
			ImGui::Text("The IP Address is usually the local address\nof the computer running the In-Sight Explorer program.");
			std::array<char, 64> ip_buffer = {};
			if (DrawInputText("IP Address", ip_buffer, m_CameraIP, 120.0f))
				m_CameraIP = std::string(ip_buffer.data());

			ImGui::SameLine();
			ImGui::Text("localhost");

			std::array<char, 64> commandPort_buffer = {};
			if (DrawInputText("Telnet Port", commandPort_buffer, m_CameraTelnetPort, 120.0f))
				m_CameraTelnetPort = std::string(commandPort_buffer.data());

			ImGui::SameLine();
			ImGui::Text("23");

			std::array<char, 64> streamPort_buffer = {};
			if (DrawInputText("TCPDevice Port", streamPort_buffer, m_CameraTCPDevicePort, 120.0f))
				m_CameraTCPDevicePort = std::string(streamPort_buffer.data());

			ImGui::SameLine();
			ImGui::Text("3000");
		});

		class Staubli {};
		DrawSection<Staubli>("Staubli Robotic Arm", [&]()
		{
			ImGui::PushFont(boldFont);
			const char* buttonText = (!m_IsRobotConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(buttonText, { 100, 50 }))
				OnRobotButtonPressed();
			ImGui::PopFont();

			ImGui::Separator();
			std::array<char, 64> ip_buffer = {};
			if (DrawInputText("IP Address", ip_buffer, m_RobotIP, 100.0f))
				m_RobotIP = std::string(ip_buffer.data());

			ImGui::SameLine();
			ImGui::Text("192.168.7.10");

			std::array<char, 64> port_buffer = {};
			if (DrawInputText("Port", port_buffer, m_RobotPort, 100.0f))
				m_RobotPort = std::string(port_buffer.data());

			ImGui::SameLine();
			ImGui::Text("15000");
		});

		class DifficultySlider {};
		DrawSection<DifficultySlider>("Engine Difficulty", [&]()
		{
			Interprocess* chessEngine = GameLayer::Get().GetConnector();

			if (DrawIntSliderControl("Skill Level", m_SkillLevel, 0, 20))
				chessEngine->SetDifficultyLevel(m_SkillLevel);

			if (DrawIntSliderControl("ELO Rating", m_ELORating, 1350, 2850))
				chessEngine->SetDifficultyELO(m_ELORating);

			static bool isELOActive;
			if (ImGui::Checkbox("Activate ELO (Overrides Skill Level)", &isELOActive))
				chessEngine->ToggleELO(isELOActive);
		});

		static glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		DrawSection<glm::vec4>("Colors", [&]()
		{
			if (DrawColorEdit4Control("Border", clearColor, 60.0f))
				m_ClearColor = clearColor * 255.0f;

			if (DrawColorEdit4Control("Odds", m_SquareColor1, 60.0f))
				OnNewSquareColor();

			if (DrawColorEdit4Control("Evens", m_SquareColor2, 60.0f))
				OnNewSquareColor();
		});

		// Display the framerate
		ImGui::Separator();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();

		ImGui::End(); // End "Settings"
    }

	void SettingsPanel::OnCameraButtonPressed()
	{
		if (m_IsCameraConnected)
		{
			GameLayer::Get().GetNetwork()->DisconnectCamera();

			const std::string msg{ "Camera disconnected." };
			LOG_INFO(msg);
			GameLayer::Get().GetConsolePanel()->AddLog(msg);

			m_IsCameraConnected = false;
		}
		else
		{
			unsigned commandThreadID{};
			_beginthreadex(NULL, 0, &Network::CameraTelnetThread, &Network::Get(), 0, &commandThreadID);

			unsigned bufferThreadID{};
			_beginthreadex(NULL, 0, &Network::CameraTCPDeviceThread, &Network::Get(), 0, &bufferThreadID);

			m_IsCameraConnected = true;
		}
	}

	void SettingsPanel::OnRobotButtonPressed()
	{
		if (m_IsRobotConnected)
		{
			GameLayer::Get().GetNetwork()->DisconnectRobot();

			const std::string msg{ "Chesster server shut down." };
			LOG_INFO(msg);
			GameLayer::Get().GetConsolePanel()->AddLog(msg);

			m_IsRobotConnected = false;
		}
		else
		{
			unsigned threadID{};
			_beginthreadex(NULL, 0, &Network::ChessterRobotThread, &Network::Get(), 0, &threadID);

			m_IsRobotConnected = true;
		}
	}

	void SettingsPanel::OnNewSquareColor()
	{
		for (Board::Square& square : Board::GetBoardSquares())
		{
			const glm::vec4 newColor = ((square.Index + 1) % 2 == 0) ? m_SquareColor1 : m_SquareColor2;
			square.Color = newColor * 255.0f;
		}
	}
}

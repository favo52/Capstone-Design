#include "pch.h"
#include "Chesster/ImGui/Panels/SettingsPanel.h"

#include "Chesster/Layers/GameLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Chesster
{
	glm::vec4 SettingsPanel::s_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };			// Black
	glm::vec4 SettingsPanel::s_SquareColor1 = { 0.084f, 0.342f, 0.517f, 1.0f };	// Blueish
	glm::vec4 SettingsPanel::s_SquareColor2 = { 1.0f, 1.0f, 1.0f, 1.0f };		// White
	
	SettingsPanel::SettingsPanel() :
		m_SkillLevel{ 0 },
		m_ELORating{ 1350 },
		m_IsNewSkillLevel{ false },
		m_IsNewELO{ false },
		m_IsToggleELOPressed{ false },
		m_IsELOActive{ false },
		m_IsCameraButtonPressed{ false },
		m_IsCameraConnected{ false },
		m_IsRobotButtonPressed{ false },
		m_IsRobotConnected{ false }
	{
	}

	void SettingsPanel::OnUpdate()
	{
		Network* TCP = GameLayer::GetTCP();

		if (m_IsCameraButtonPressed)
		{
			if (m_IsCameraConnected)
			{
				TCP->DisconnectCamera();
				m_IsCameraConnected = false;
			}
			else
			{
				m_IsCameraConnected = true;
				TCP->ConnectCamera();
				TCP->SendCameraCommand("SE8");
				if (!TCP->RecvCameraConfirmation())
				{
					LOG_WARN("Camera did not connect sucessfully.");
					GameLayer::GetConsolePanel()->AddLog("Camera did not connect sucessfully.");
					m_IsCameraConnected = false;
				}
			}

			m_IsCameraButtonPressed = false;
		}

		if (m_IsRobotButtonPressed)
		{
			if (m_IsRobotConnected)
			{
				TCP->DisconnectRobot();
				m_IsRobotConnected = false;
			}
			else
			{
				m_IsRobotConnected = true;

				unsigned threadID{};
				_beginthreadex(NULL, 0, &Network::ConnectRobotThread, &Network::Get(), 0, &threadID);
			}

			m_IsRobotButtonPressed = false;
		}

		Interprocess* Connector = GameLayer::Get().GetConnector();

		if (m_IsNewSkillLevel)
		{
			Connector->SetDifficultyLevel(m_SkillLevel);
			m_IsNewSkillLevel = false;
		}

		if (m_IsNewELO)
		{
			Connector->SetDifficultyELO(m_ELORating);
			m_IsNewELO = false;
		}

		if (m_IsToggleELOPressed)
		{
			Connector->ToggleELO(m_IsELOActive);
			m_IsToggleELOPressed = false;
		}
	}

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

		Network& TCP = Network::Get();
		auto boldFont = ImGui::GetIO().Fonts->Fonts[2];

		class Cognex {};
		DrawSection<Cognex>("Cognex Camera", [&]()
		{
			ImGui::PushFont(boldFont);
			const char* buttonText = (!m_IsCameraConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(buttonText, { 100, 50 }))
				m_IsCameraButtonPressed = true;
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
			const char* buttonText = (!m_IsRobotConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(buttonText, { 100, 50 }))
				m_IsRobotButtonPressed = true;
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
		DrawSection<DifficultySlider>("Engine Difficulty", [&]()
		{
			DrawIntControl("Skill Level", m_SkillLevel, m_IsNewSkillLevel, 0, 20);
			DrawIntControl("ELO Rating", m_ELORating, m_IsNewELO, 1350, 2850);

			if (ImGui::Checkbox("Activate ELO (Overrides Skill Level)", &m_IsELOActive))
				m_IsToggleELOPressed = true;
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

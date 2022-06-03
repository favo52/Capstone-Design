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
#include "Chesster/ImGui/SettingsPanel.h"

#include "Chesster/Layers/GameLayer.h"
#include "Chesster/Connections/ChessEngine.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Chesster
{	
	SettingsPanel::SettingsPanel() :
		m_CameraIP{ "192.168.7.9" },
		m_CameraTelnetPort{ "23" },
		m_CameraTCPDevicePort{ "3000" },
		m_ServerIP{ "192.168.7.10" },
		m_ServerPort{ "15000" },
		m_SkillLevel{ 0 },
		m_ELORating{ 1350 },
		m_MultiPV{ 1 },
		m_Depth{ 5 },
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

		if (!ImGui::SliderInt("##Diff", &value, min, max, "%d", ImGuiSliderFlags_AlwaysClamp)
			&& ImGui::IsItemDeactivatedAfterEdit())
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

	template<size_t S>
	static bool DrawInputText(const std::string& label, std::array<char, S>& buffer, const std::string& text, float columnWidth = 100.0f)
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
			ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
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

		auto boldFont = ImGui::GetIO().Fonts->Fonts[2];

		ImGui::PushFont(boldFont);

		const char* cameraButtonText = (!m_IsCameraConnected) ? "Connect Camera" : "Disconnect Camera";
		if (ImGui::Button(cameraButtonText, { 150, 80 }))
			OnCameraButtonPressed();

		ImGui::SameLine();

		const char* robotButtonText = (!m_IsRobotConnected) ? " Start Server" : " Stop Server";
		if (ImGui::Button(robotButtonText, { 150, 80 }))
			OnRobotButtonPressed();

		ImGui::PopFont();

		class Cognex {};
		DrawSection<Cognex>("Cognex Camera", [&]()
		{
			ImGui::BeginDisabled(!m_IsCameraConnected);

			ImGui::PushFont(boldFont);
			if (ImGui::Button("Take Picture", { 100, 50 }))
				GameLayer::Get().GetNetwork().SendToCamera("SE8\r\n");

			ImGui::EndDisabled();
			ImGui::PopFont();

			ImGui::Separator();
			ImGui::Text("The IP Address is the Static IP assigned to the camera.\n"
						"It can be verified in the In-Sight Explorer program.\n"
						"The camera must act as a server.");
			std::array<char, 32> buffer = {};
			if (DrawInputText("IP Address", buffer, m_CameraIP, 120.0f))
				m_CameraIP = std::string(buffer.data());

			ImGui::SameLine();
			ImGui::Text("192.168.7.9");

			if (DrawInputText("Telnet Port", buffer, m_CameraTelnetPort, 120.0f))
				m_CameraTelnetPort = std::string(buffer.data());

			ImGui::SameLine();
			ImGui::Text("23");

			if (DrawInputText("TCPDevice Port", buffer, m_CameraTCPDevicePort, 120.0f))
				m_CameraTCPDevicePort = std::string(buffer.data());

			ImGui::SameLine();
			ImGui::Text("3000");
		});

		class Staubli {};
		DrawSection<Staubli>("Staubli Robotic Arm", [&]()
		{
			ImGui::Separator();
			ImGui::Text("The Chesster application acts as a server.\n"
				"The Staubli robot arm acts as a client.");
			std::array<char, 32> buffer = {};
			if (DrawInputText("IP Address", buffer, m_ServerIP, 100.0f))
				m_ServerIP = std::string(buffer.data());

			ImGui::SameLine();
			ImGui::Text("192.168.7.10");

			if (DrawInputText("Port", buffer, m_ServerPort, 100.0f))
				m_ServerPort = std::string(buffer.data());

			ImGui::SameLine();
			ImGui::Text("15000");
		});

		class DifficultySlider {};
		DrawSection<DifficultySlider>("Engine Difficulty", [&]()
		{
			ChessEngine& chessEngine = GameLayer::Get().GetChessEngine();
			
			static bool isELOActive{ true };

			if (DrawIntSliderControl("Depth", m_Depth, 1, 25))
			{
				LOG_INFO("Depth set to {0}.", m_Depth);
				GameLayer::Get().GetConsolePanel().AddLog("Depth set to " + std::to_string(m_Depth) + ".");
			}
			ImGui::Text("Depth indicates the number of half\n"
						"moves the engine looks ahead.");

			ImGui::Separator();

			ImGui::BeginDisabled(isELOActive);
			if (DrawIntSliderControl("Skill Level", m_SkillLevel, 0, 20))
				chessEngine.SetDifficultyLevel(m_SkillLevel);			
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!isELOActive);
			if (DrawIntSliderControl("ELO Rating", m_ELORating, 1350, 2850))
				chessEngine.SetDifficultyELO(m_ELORating);

			if (DrawIntSliderControl("MultiPV", m_MultiPV, 1, 10))
				chessEngine.SetMultiPV(m_MultiPV);
			ImGui::EndDisabled();

			ImGui::PushFont(boldFont);
			ImGui::Text("MultiPV (Principal Variation) increases the chance\nof a random move, making the game engine play\nweaker. (ELO only)");
			ImGui::PopFont();

			if (ImGui::Checkbox("Activate ELO (Overrides Skill Level)", &isELOActive))
				chessEngine.ToggleELO(isELOActive);
		});

		static glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		class Board {};
		DrawSection<Board>("Board", [&]()
		{
			ImGui::Separator();
			ImGui::Text("Colors");
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

		// Activate/deactivate mouse picking
		static bool isMouseActive{ false };
		if (ImGui::Checkbox("Allow Mouse/Keyboard events", &isMouseActive))
			GameLayer::Get().SetEventsActive(isMouseActive);
		ImGui::Text("Click pieces for drag and drop.\nPress LeftCtrl + Spacebar for computer play.");

		ImGui::End(); // End "Settings"
    }

	void SettingsPanel::OnCameraButtonPressed()
	{
		if (m_IsCameraConnected)
		{
			GameLayer::Get().GetNetwork().ShutdownCamera();

			const std::string msg{ "Camera disconnected." };
			LOG_INFO(msg);
			GameLayer::Get().GetLogPanel().AddLog(msg);

			m_IsCameraConnected = false;
		}
		else
		{
			std::thread(Network::CameraTelnetThread).detach();
			std::thread(Network::CameraTCPDeviceThread).detach();
			
			m_IsCameraConnected = true;
		}
	}

	void SettingsPanel::OnRobotButtonPressed()
	{
		if (m_IsRobotConnected)
		{
			GameLayer::Get().GetNetwork().DisconnectRobot();

			const std::string msg{ "Chesster server shut down." };
			LOG_INFO(msg);
			GameLayer::Get().GetLogPanel().AddLog(msg);

			m_IsRobotConnected = false;
		}
		else
		{
			std::thread(Network::ChessterServerThread).detach();
			m_IsRobotConnected = true;
		}
	}

	void SettingsPanel::OnNewSquareColor()
	{
		auto& boardSquares = GameLayer::Get().GetBoard().GetBoardSquares();

		for (Board::Square& square : boardSquares)
		{
			const glm::vec4 newColor =
				((square.Notation[0] + square.Notation[1]) % 2 == 0) ? m_SquareColor1 : m_SquareColor2;

			square.Color = newColor * 255.0f;
		}
	}
}

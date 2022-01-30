#include "pch.h"
#include "Chesster_Unleashed/ImGui/Panels/SettingsPanel.h"

#include "Chesster_Unleashed/Game/Board.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Chesster
{
	glm::vec4 SettingsPanel::ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black

	int SettingsPanel::SkillLevel{ 0 };
	int SettingsPanel::ELO{ 1350 };

	bool SettingsPanel::IsNewSkillLevel{ false };
	bool SettingsPanel::IsNewELO{ false };
	bool SettingsPanel::IsToggleELO{ false };
	bool SettingsPanel::IsELOActive{ false };

	bool SettingsPanel::IsCameraButton{ false };
	bool SettingsPanel::IsCameraConnected{ false };

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

	template<typename T, typename UIFunction>
	static void DrawSection(const std::string& name, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
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

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[2];

		class Cognex {};
		DrawSection<Cognex>("Cognex Camera", [boldFont]()
		{
			ImGui::PushFont(boldFont); 
			const char* button = (!IsCameraConnected) ? "Connect" : "Disconnect";
			if (ImGui::Button(button, { 100, 50 }))
			{
				if (!IsCameraConnected) IsCameraButton = true;
				if (IsCameraConnected) IsCameraConnected = false;
			}
			ImGui::PopFont();
		});

		class Staubli {};
		DrawSection<Staubli>("Staubli Robotic Arm", []()
		{

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
				ClearColor = clearColor * 255.0f;

			if (DrawColorEdit4Control("Odds", SquareColor1, 60.0f))
				UpdateSquareColors();

			if (DrawColorEdit4Control("Evens", SquareColor2, 60.0f))
				UpdateSquareColors();
		});

		ImGui::Separator();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::End(); // End "Settings"
    }

	void SettingsPanel::UpdateSquareColors()
	{
		for (Board::Square& square : Board::GetBoardSquares())
		{
			glm::vec4 newColor = ((square.Index + 1) % 2 == 0) ? SquareColor1 : SquareColor2;
			square.Color = newColor * 255.0f;
		}
	}
}

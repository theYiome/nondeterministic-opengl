#pragma once
#include <imgui.h>
#include "examples.hpp"

namespace imgui_utils {
    void render(CameraWindow& window) {
        {
            ImGui::Begin("Window configuration");
            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
            if (ImGui::TreeNode("Display mode")) {
                ImGui::Text("[F11] - enter/exit fullscreen mode");
                ImGui::Spacing();
                window.window.imgui_panel();
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Camera")) {
                ImGui::Text("        [F1] - enter/exit camera mode");
                ImGui::Text("[W][A][S][D] - move around the scene");
                ImGui::Spacing();
                window.camera.imgui_panel();
                ImGui::TreePop();
            }
            ImGui::End();
        }
    }
}
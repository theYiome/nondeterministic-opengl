#include <imgui.h>
#include "FirstPersonCamera.hpp"

void FirstPersonCamera::update_look_at() {
    glm::vec3 direction;
    direction.x = cos(yaw) * cos(pitch);
    direction.y = sin(pitch);
    direction.z = sin(yaw) * cos(pitch);
    look_at = glm::normalize(direction);
}

void FirstPersonCamera::update_pitch(const float& pitch_difference) {
    pitch += pitch_difference * sensitivity;
    if (pitch > max_pitch)
        pitch = max_pitch;
    else if (pitch < -max_pitch)
        pitch = -max_pitch;
}

void FirstPersonCamera::update_yaw(const float& yaw_difference) {
    yaw += yaw_difference * sensitivity;
    if (yaw > max_yaw)
        yaw -= max_yaw;
    else if (yaw < -max_yaw)
        yaw += max_yaw;
}

void FirstPersonCamera::imgui_panel() {
    ImGui::SliderFloat("Sensitivity", &sensitivity, 0.001f, 0.01f);
    ImGui::SliderFloat("Speed", &speed, 0.f, 100.f);
    ImGui::SliderFloat("FOV", &FOV, 0, glm::pi<float>());
    ImGui::SliderFloat("Pitch", &pitch, -max_pitch, max_pitch);
    ImGui::DragFloat("Yaw", &yaw, 0.01f);
    ImGui::DragFloat3("Postition", glm::value_ptr(position), 0.05f);
    ImGui::DragFloat3("Look at", glm::value_ptr(look_at), 0.05f);
    ImGui::DragFloat3("Up unit vector", glm::value_ptr(up), 0.05f);
}
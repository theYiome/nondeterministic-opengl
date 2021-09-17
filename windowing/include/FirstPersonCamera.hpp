#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class FirstPersonCamera {

public:
    float sensitivity = 0.003f;
    float speed = 10.f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 look_at = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float FOV = glm::pi<float>() * 0.5f;

    float pitch = 0;
    float yaw = glm::pi<float>() * (-0.5f);

    float max_pitch = glm::pi<float>() * 0.45f;
    float max_yaw = glm::pi<float>() * 2.f;

    void update_look_at();
    void update_pitch(const float& pitch_difference = 0.f);
    void update_yaw(const float& yaw_difference = 0.f);
    void imgui_panel();
};
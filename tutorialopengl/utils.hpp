#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CameraWindow.hpp>
#include "examples.hpp"


namespace transform {

    glm::mat4 projection(const CameraWindow& window) {
        return glm::perspective<float>(window.camera.FOV, float(window.window.width) / window.window.height, 0.1f, 500.f);
    }

    glm::mat4 view(const CameraWindow& window) {
        return glm::lookAt(window.camera.position, window.camera.look_at + window.camera.position, window.camera.up);
    }
}


float calculate_dt() {
    static float last_time = static_cast<float>(glfwGetTime());
    float current_time = static_cast<float>(glfwGetTime());
    float dt = current_time - last_time;
    last_time = current_time;
    return dt;
}
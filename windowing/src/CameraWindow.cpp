#pragma once
#include <string>

#include "CameraWindow.hpp"
#include "Callbacks.hpp"

void CameraWindow::update_camera_postition(const float& dt) {
    const float dx = camera.speed * dt;
    GLFWwindow* ptr = window.window_ptr;
    if (glfwGetKey(ptr, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += dx * camera.look_at;
    if (glfwGetKey(ptr, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= dx * camera.look_at;
    if (glfwGetKey(ptr, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.look_at, camera.up)) * dx;
    if (glfwGetKey(ptr, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.look_at, camera.up)) * dx;
}

void CameraWindow::set_callbacks() {
    glfwSetWindowUserPointer(window.window_ptr, this);
    glfwSetKeyCallback(window.window_ptr, callbacks::on_key_press);
    glfwSetCursorPosCallback(window.window_ptr, callbacks::on_mouse_move);
    glfwSetWindowSizeCallback(window.window_ptr, callbacks::on_viewport_change);
}

CameraWindow::CameraWindow(const std::string& name, const int& window_width, const int& window_height) :
    window(name, window_width, window_height) {
    
    set_callbacks();
}

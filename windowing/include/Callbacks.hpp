#pragma once
#include "CameraWindow.hpp"

namespace callbacks {
    CameraWindow* get_this(GLFWwindow* window);

    void on_viewport_change(GLFWwindow* window, int w, int h);

    void on_key_press(GLFWwindow* window, int key, int scancode, int action, int mods);

    void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
}
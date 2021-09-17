#include <GLAD/glad.h>

#include "CameraWindow.hpp"

namespace callbacks {
    CameraWindow* get_this(GLFWwindow* window) {
        return static_cast<CameraWindow*>(glfwGetWindowUserPointer(window));
    }

    void on_viewport_change(GLFWwindow* window, int w, int h) {
        CameraWindow* ptr = get_this(window);
        ptr->window.width = w;
        ptr->window.height = h;
        glViewport(0, 0, w, h);
    }

    void on_key_press(GLFWwindow* window, int key, int scancode, int action, int mods) {
        CameraWindow* ptr = static_cast<CameraWindow*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
            ptr->window.toggle_cursor_mode();

        if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
            if (ptr->window.selected_monitor == ptr->window.default_display_mode) {
                std::string first_label = ptr->window.labeled_monitors.begin()->first;
                ptr->window.set_fullscreen(first_label);
            }
            else
                ptr->window.set_fullscreen(ptr->window.default_display_mode);

        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
        CameraWindow* ptr = get_this(window);
        bool& was_disabled_last_frame = ptr->camera_disabled_last_frame;
        static glm::vec2 last_position;
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            const glm::vec2 current_position(xpos, ypos);
            if (was_disabled_last_frame) {
                const glm::vec2 diff = current_position - last_position;
                ptr->camera.update_pitch(-diff.y);
                ptr->camera.update_yaw(diff.x);
            }
            last_position = current_position;
            was_disabled_last_frame = true;
        }
        else
            was_disabled_last_frame = false;
    }
}
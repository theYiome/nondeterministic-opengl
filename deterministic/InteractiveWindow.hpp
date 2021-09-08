#pragma once
#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace window {

    GLFWwindow* glew_window;
    int width = 900;
    int height = 900;

    std::vector<std::string> monitor_labels;
    GLFWmonitor** monitors;
    GLFWmonitor* current_monitor;
    static const char* current_monitor_label = nullptr;

    float color_array[]{ 0.7f, 0.1f, 0.2f };
    static std::map<int, GLenum> polygon_modes_map{ {0, GL_FILL}, {1, GL_LINE}, {2, GL_POINT} };
    static int current_polygon_mode = 0;

    GLuint uniform_factor;
    GLuint uniform_rotation;
    GLuint uniform_transformation;

    namespace camera {
        float sensitivity = 0.003f;
        float speed = 10.f;

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 look_at = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        float FOV = glm::pi<float>() / 2.f;
        float pitch = 0;
        float yaw = glm::pi<float>() * (-0.5f);

        constexpr float max_pitch = glm::pi<float>() * 0.49f;
        constexpr float max_yaw = glm::pi<float>() * 2.f;

        void update_postition(GLFWwindow* window, const float& dt) {
            const float diff = speed * dt;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                position += diff * look_at;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                position -= diff * look_at;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                position -= glm::normalize(glm::cross(look_at, up)) * diff;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                position += glm::normalize(glm::cross(look_at, up)) * diff;
        }

        void update_look_at() {
            glm::vec3 direction;
            direction.x = cos(yaw) * cos(pitch);
            direction.y = sin(pitch);
            direction.z = sin(yaw) * cos(pitch);
            look_at = glm::normalize(direction);
        }

        void update_pitch(const float& pitch_difference = 0.f) {
            pitch += pitch_difference * sensitivity;
            if (pitch > max_pitch)
                pitch = max_pitch;
            else if (pitch < -max_pitch)
                pitch = -max_pitch;
        }

        void update_yaw(const float& yaw_difference = 0.f) {
            yaw += yaw_difference * sensitivity;
            if (yaw > max_yaw)
                yaw -= max_yaw;
            else if (yaw < -max_yaw)
                yaw += max_yaw;
        }
    }


    void set_fullscreen(GLFWwindow* window, GLFWmonitor* monitor = NULL) {
        if (monitor != NULL) {
            glfwSetWindowMonitor(window, monitor, 0, 0, 15360, 8640, GLFW_DONT_CARE);
        }
        else {
            if (glfwGetWindowMonitor(window) == NULL) {
                monitor = glfwGetPrimaryMonitor();
                glfwSetWindowMonitor(window, monitor, 0, 0, 15360, 8640, GLFW_DONT_CARE);
                current_monitor_label = monitor_labels[1].c_str();
            }
            else {
                glfwSetWindowMonitor(window, NULL, 100, 100, 800, 800, GLFW_DONT_CARE);
                current_monitor_label = monitor_labels[0].c_str();
            }
        }
    }

    void toggle_cursor_mode(GLFWwindow* window) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // enable mouse capture by ImGUI
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // disable mouse capture by ImGUI
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
        }
    }

    void on_viewport_change(GLFWwindow* window, int w, int h) {
        width = w;
        height = h;
        glViewport(0, 0, width, height);
    }

    void on_monitor_change(GLFWmonitor* monitor = nullptr, int monitor_event = 0) {
        int count;
        monitors = glfwGetMonitors(&count);

        monitor_labels.clear();
        monitor_labels.push_back("Windowed");
        for (int i = 0; i < count; ++i) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[0]);

            std::ostringstream string_stream;
            string_stream << "Fullscreen on monitor " << i + 1 << ": ";
            string_stream << mode->width << "x" << mode->height << " " << mode->refreshRate << "Hz";
            monitor_labels.push_back(string_stream.str());
        }
        current_monitor_label = monitor_labels[0].c_str();
    }

    void on_key_press(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
            toggle_cursor_mode(window);
        }

        if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
            set_fullscreen(window);
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
        static bool was_disabled_last_frame = false;
        static glm::vec2 last_position;

        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            const glm::vec2 current_position(xpos, ypos);

            if (was_disabled_last_frame) {
                const glm::vec2 diff = current_position - last_position;
                camera::update_pitch(-diff.y);
                camera::update_yaw(diff.x);
            }

            last_position = current_position;
            was_disabled_last_frame = true;
        }
        else {
            was_disabled_last_frame = false;
        }
    }

    void initialize(GLFWwindow* window) {
        glew_window = window;
        glfwSetFramebufferSizeCallback(window, on_viewport_change);
        glfwSetCursorPosCallback(window, on_mouse_move);
        glfwSetKeyCallback(window, on_key_press);
        glfwSetMonitorCallback(on_monitor_change);

        on_monitor_change();
        current_monitor_label = monitor_labels[0].c_str();

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
    }
}


void app_config_window() {
    ImGui::Begin("App Config");

    ImGui::Spacing();
    ImGui::Text("       [F11] - enter/exit fullscreen mode");
    ImGui::Text("        [F1] - enter/exit camera mode");
    ImGui::Text("[W][A][S][D] - move around the scene");
    ImGui::Spacing();

    ImGui::ColorEdit3("Background color", window::color_array);

    {
        const char* const labels[] = { "GL_FILL", "GL_LINE", "GL_POINT" };
        ImGui::Combo("Drawing mode", &window::current_polygon_mode, labels, IM_ARRAYSIZE(labels));
    }

    {
        static float factor = 0.5f;
        if (ImGui::SliderFloat("Factor", &factor, -2.f, 2.f)) {
            glUniform1f(window::uniform_factor, factor);
        }
    }

    {
        const std::vector<std::string>& labels = window::monitor_labels;
        //const char* &current_item = window::current_monitor_label;

        // The second parameter is the label previewed before opening the combo.
        if (ImGui::BeginCombo("Display mode", window::current_monitor_label))
        {
            for (int i = 0; i < labels.size(); ++i) {
                bool is_selected = (window::current_monitor_label == labels[i].c_str());
                if (ImGui::Selectable(labels[i].c_str(), is_selected)) {
                    window::current_monitor_label = labels[i].c_str();

                    if (i == 0) window::set_fullscreen(window::glew_window, NULL);
                    else window::set_fullscreen(window::glew_window, window::monitors[i - 1]);
                }
                // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::TreeNode("Camera")) {
        {
            ImGui::SliderFloat("Sensitivity", &window::camera::sensitivity, 0.001f, 0.01f);
            ImGui::SliderFloat("Speed", &window::camera::speed, 0.f, 100.f);
            ImGui::SliderFloat("FOV", &window::camera::FOV, 0, glm::pi<float>());
            ImGui::SliderFloat("Pitch", &window::camera::pitch, -window::camera::max_pitch, window::camera::max_pitch);
            ImGui::DragFloat("Yaw", &window::camera::yaw, 0.01f);
            ImGui::DragFloat3("Postition", glm::value_ptr(window::camera::position), 0.05f);
            ImGui::DragFloat3("Look at", glm::value_ptr(window::camera::look_at), 0.05f);
            ImGui::DragFloat3("Up unit vector", glm::value_ptr(window::camera::up), 0.05f);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Special cube")) {
        {
            static float rotation = -0.7f;
            static float scale[3] = { 1.0f, 1.0f, 1.0f };
            static float translate[3] = { 0.f, 0.f, -3.f };

            const float range = 3.14f * 2.f;

            ImGui::SliderFloat("Rotation", &rotation, -range, range);
            ImGui::DragFloat3("Scale", scale, 0.05f);
            ImGui::DragFloat3("Translate", translate, 0.05f);

            glm::mat4 transformation = glm::mat4(1.0f);
            glm::mat4 model = glm::rotate(glm::mat4(1.f), rotation, glm::vec3(0.9f, 0.1f, 0.1f));
            model *= glm::scale(transformation, glm::vec3(scale[0], scale[1], scale[2]));
            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(translate[0], translate[1], translate[2]));
            glm::mat4 projection = glm::perspective<float>(window::camera::FOV, float(window::width) / window::height, 0.1f, 100.0f);

            transformation = transformation * projection * view * model;
            glUniformMatrix4fv(window::uniform_transformation, 1, GL_FALSE, glm::value_ptr(transformation));
        }

        ImGui::TreePop();
    }

    {
        const char* const labels[] = { "GL_FILL", "GL_LINE", "GL_POINT" };
        ImGui::Combo("Polygon mode", &window::current_polygon_mode, labels, IM_ARRAYSIZE(labels));
    }

    //if (ImGui::TreeNode("Data")) {
    //    {
    //        ImTextureID wall_texture = (void*)(intptr_t)wall_texture_id;
    //        float my_tex_w = static_cast<float>(wall_texture_width);
    //        float my_tex_h = static_cast<float>(wall_texture_height);
    //        ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
    //        ImGui::Image(wall_texture, ImVec2(my_tex_w, my_tex_h));
    //    }

    //    ImGui::TreePop();
    //}

    ImGui::End();
}
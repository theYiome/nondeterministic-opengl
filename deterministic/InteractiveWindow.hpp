#pragma once
#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "main.hpp"
#include "Texture.hpp"


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

    void imgui_panel() {
        if (ImGui::TreeNode("Camera")) {
            ImGui::SliderFloat("Sensitivity", &sensitivity, 0.001f, 0.01f);
            ImGui::SliderFloat("Speed", &speed, 0.f, 100.f);
            ImGui::SliderFloat("FOV", &FOV, 0, glm::pi<float>());
            ImGui::SliderFloat("Pitch", &pitch, -max_pitch, max_pitch);
            ImGui::DragFloat("Yaw", &yaw, 0.01f);
            ImGui::DragFloat3("Postition", glm::value_ptr(position), 0.05f);
            ImGui::DragFloat3("Look at", glm::value_ptr(look_at), 0.05f);
            ImGui::DragFloat3("Up unit vector", glm::value_ptr(up), 0.05f);
        }
        ImGui::TreePop();
    }
};
 

class SimpleWindow {
    static constexpr int std_width = 900;
    static constexpr int std_height = 900;
public:
    GLFWwindow* const window_ptr;
    int width = 900;
    int height = 900;

    std::map<std::string, GLFWmonitor*> labeled_monitors;
    std::string selected_monitor;

    static void init_glfw() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    SimpleWindow(const std::string& name, const int& window_width = std_width, const int& window_height = std_height) :
        window_ptr(glfwCreateWindow(width, height, name.c_str(), NULL, NULL)) {

        glfwMakeContextCurrent(window_ptr);
        //glfwSetFramebufferSizeCallback(window_ptr, on_viewport_change);
        //glfwSetCursorPosCallback(window_ptr, on_mouse_move);
        //glfwSetKeyCallback(window_ptr, on_key_press);
        //glfwSetMonitorCallback(on_monitor_change);
        //on_monitor_change();

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        update_monitors();
    }

    static std::string create_monitor_label(GLFWmonitor* const monitor, const int& index = 0) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        std::ostringstream string_stream;
        string_stream << "Fullscreen on monitor " << index + 1 << ": ";
        string_stream << mode->width << "x" << mode->height << " " << mode->refreshRate << "Hz";
        return string_stream.str();
    }

    void update_monitors() {
        labeled_monitors.clear();
        labeled_monitors.insert({ std::string("Windowed"), nullptr });
        // pool GLFW for connected monitors and add them to labeled_monitors
        {
            int count;
            GLFWmonitor** monitors = glfwGetMonitors(&count);
            for (int index = 0; index < count; ++index) {
                std::string label = create_monitor_label(monitors[index], index);
                labeled_monitors.insert({ label, monitors[index] });
            }
        }
    }

    void set_fullscreen(const std::string& label) {
        const int big_width = 15360;
        const int big_height = 8640;

        GLFWmonitor* monitor = labeled_monitors[label];
        if (monitor)
            glfwSetWindowMonitor(window_ptr, monitor, 0, 0, big_width, big_height, GLFW_DONT_CARE);
        else
            glfwSetWindowMonitor(window_ptr, nullptr, 100, 100, std_width, std_height, GLFW_DONT_CARE);

        selected_monitor = label;
    }

    void imgui_panel() {
        if (ImGui::BeginCombo("Display mode", selected_monitor.c_str())) {


            for (const auto& pair : labeled_monitors) {
                bool selected = pair.first == selected_monitor;
                if (ImGui::Selectable(pair.first.c_str(), selected)) {
                    std::cout << pair.first << std::endl;
                }
            }

            //for (int i = 0; i < labels.size(); ++i) {
            //    const char* choosen_label = labels[i].c_str();
            //    bool is_selected = (window::current_monitor_label == choosen_label);
            //    if (ImGui::Selectable(choosen_label, is_selected)) {
            //        window::current_monitor_label = choosen_label;
            //        if (i == 0) window::set_fullscreen(window::glew_window, NULL);
            //        else window::set_fullscreen(window::glew_window, window::monitors[i - 1]);
            //    }
            //    // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            //    //if (is_selected) ImGui::SetItemDefaultFocus();
            //}
        }
        ImGui::EndCombo();
    }
};


class CameraWindow {
    static constexpr int std_width = 900;
    static constexpr int std_height = 900;
public:
    SimpleWindow window;
    FirstPersonCamera camera;

    CameraWindow(const std::string& name, const int& window_width = std_width, const int& window_height = std_height) :
        window(name, window_width, window_height) {
    }

    void update_camera_postition(const float& dt) {
        const float dx = camera.speed * dt;
        if (glfwGetKey(window.window_ptr, GLFW_KEY_W) == GLFW_PRESS)
            camera.position += dx * camera.look_at;
        if (glfwGetKey(window.window_ptr, GLFW_KEY_S) == GLFW_PRESS)
            camera.position -= dx * camera.look_at;
        if (glfwGetKey(window.window_ptr, GLFW_KEY_A) == GLFW_PRESS)
            camera.position -= glm::normalize(glm::cross(camera.look_at, camera.up)) * dx;
        if (glfwGetKey(window.window_ptr, GLFW_KEY_D) == GLFW_PRESS)
            camera.position += glm::normalize(glm::cross(camera.look_at, camera.up)) * dx;
    }
};

namespace window {

    GLFWwindow* glew_window;
    int width = 900;
    int height = 900;

    std::vector<std::string> monitor_labels;
    GLFWmonitor** monitors;
    GLFWmonitor* current_monitor;
    static const char* current_monitor_label = nullptr;

    std::shared_ptr<AppData> app_data;

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


void app_data_window() {
    ImGui::Begin("App Config");

    ImGui::Spacing();
    ImGui::Text("       [F11] - enter/exit fullscreen mode");
    ImGui::Text("        [F1] - enter/exit camera mode");
    ImGui::Text("[W][A][S][D] - move around the scene");
    ImGui::Spacing();

    ImGui::ColorEdit3("Background color", glm::value_ptr(window::app_data->bg_color));

    {
        const char* const labels[] = { "GL_FILL", "GL_LINE", "GL_POINT" };
        ImGui::Combo("Drawing mode", &window::app_data->current_polygon_mode, labels, IM_ARRAYSIZE(labels));
    }

    {
        float& color_ratio = window::app_data->color_ratio;
        if (ImGui::SliderFloat("Color (texture -> vertex)", &color_ratio, 0.f, 1.f)) {
            glUniform1f(window::app_data->uniforms.factor, color_ratio);
        }
    }

    {
        const std::vector<std::string>& labels = window::monitor_labels;
        //const char* &current_item = window::current_monitor_label;

        // The second parameter is the label previewed before opening the combo.
        if (ImGui::BeginCombo("Display mode", window::current_monitor_label))
        {
            for (int i = 0; i < labels.size(); ++i) {
                const char* choosen_label = labels[i].c_str();

                bool is_selected = (window::current_monitor_label == choosen_label);
                if (ImGui::Selectable(choosen_label, is_selected)) {
                    window::current_monitor_label = choosen_label;

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
    {
        const char* const labels[] = { "GL_FILL", "GL_LINE", "GL_POINT" };
        ImGui::Combo("Polygon mode", &window::app_data->current_polygon_mode, labels, IM_ARRAYSIZE(labels));
    }

    if (ImGui::TreeNode("Data")) {
        {
            const Texture& wall_tex = window::app_data->textures.wall;
            ImTextureID wall_texture = (void*)(intptr_t)wall_tex.id;
            ImGui::Text("%d x %d", wall_tex.width, wall_tex.height);
            ImGui::Image(wall_texture, ImVec2(wall_tex.width, wall_tex.height));
        }
        ImGui::TreePop();
    }

    ImGui::End();
}



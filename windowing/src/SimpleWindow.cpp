#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

#include <imgui.h>

#include "SimpleWindow.hpp"


void SimpleWindow::init_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void SimpleWindow::toggle_cursor_mode() {
    if (glfwGetInputMode(window_ptr, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // enable mouse capture by ImGUI
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }
    else {
        glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // disable mouse capture by ImGUI
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
}

SimpleWindow::SimpleWindow(const std::string& name, const int& window_width, const int& window_height) :
    window_ptr(glfwCreateWindow(window_width, window_height, name.c_str(), NULL, NULL)) {
    assert(window_ptr != nullptr);
    update_monitors();
    glfwMakeContextCurrent(window_ptr);
}

SimpleWindow::~SimpleWindow() {
    glfwTerminate();
}

std::string SimpleWindow::create_monitor_label(GLFWmonitor* const monitor, const int& index) {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    std::ostringstream string_stream;
    string_stream << "Fullscreen on monitor " << index + 1 << ": ";
    string_stream << mode->width << "x" << mode->height << " " << mode->refreshRate << "Hz";
    return string_stream.str();
}

void SimpleWindow::update_monitors() {
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

void SimpleWindow::set_fullscreen(const std::string& label) {
    const int big_width = 15360;
    const int big_height = 8640;

    GLFWmonitor* monitor = labeled_monitors[label];
    if (monitor)
        glfwSetWindowMonitor(window_ptr, monitor, 0, 0, big_width, big_height, GLFW_DONT_CARE);
    else
        glfwSetWindowMonitor(window_ptr, nullptr, 100, 100, std_width, std_height, GLFW_DONT_CARE);

    selected_monitor = label;
}

void SimpleWindow::imgui_panel() {
    if (ImGui::BeginCombo("Display mode", selected_monitor.c_str())) {

        update_monitors();
        for (const auto& pair : labeled_monitors) {
            bool selected = pair.first == selected_monitor;
            //selected = true;
            if (ImGui::Selectable(pair.first.c_str(), selected)) {
                selected_monitor = pair.first;
                std::cout << selected_monitor << std::endl;
                set_fullscreen(selected_monitor);
            }
        }

        ImGui::EndCombo();
    }
}
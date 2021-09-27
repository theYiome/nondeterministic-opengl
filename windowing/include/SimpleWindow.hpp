#pragma once
#include <map>

#include <GLFW/glfw3.h>


class SimpleWindow {
public:
    static constexpr int std_width = 900;
    static constexpr int std_height = 900;
    static constexpr const char* default_display_mode = "Windowed";
    
    GLFWwindow* const window_ptr;
    int width;
    int height;

    std::map<std::string, GLFWmonitor*> labeled_monitors;
    std::string selected_monitor = default_display_mode;

    static void init_glfw();

    void toggle_cursor_mode();

    SimpleWindow(const std::string& name, const int& window_width = std_width, const int& window_height = std_height);

    ~SimpleWindow();

    static std::string create_monitor_label(GLFWmonitor* const monitor, const int& index = 0);

    void update_monitors();

    void set_fullscreen(const std::string& label);

    void init_imgui();

    void terminate_imgui();

    void start_frame();

    void end_frame();

    void imgui_panel();
};
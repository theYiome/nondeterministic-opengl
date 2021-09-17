#pragma once
#include <string>

#include "FirstPersonCamera.hpp"
#include "SimpleWindow.hpp"

class CameraWindow {

public:
    SimpleWindow window;

    FirstPersonCamera camera;
    bool camera_disabled_last_frame = false;

    void update_camera_postition(const float& dt);

    void set_callbacks();

    CameraWindow(const std::string& name, const int& window_width = SimpleWindow::std_width, const int& window_height = SimpleWindow::std_height);
};
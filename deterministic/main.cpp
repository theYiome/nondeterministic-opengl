#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>

#include <stb_image.h>
#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "utils_gl.hpp"


namespace window {

    int width = 900;
    int height = 900;

    bool cursor_disabled = false;

    namespace camera {
        float sensitivity = 0.004f;
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 look_at = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        float pitch = 0;
        float yaw = glm::pi<float>() * (-0.5f);

        constexpr float max_pitch = glm::pi<float>() * 0.49f;

        void update_postition(GLFWwindow* window, const float &dt, const float &speed = 4.f) {
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

        void update_pitch_yaw(const float &pitch_diff = 0.f, const float &yaw_diff = 0.f) {
            pitch += pitch_diff * sensitivity;
            yaw += yaw_diff * sensitivity;

            if (pitch > max_pitch)
                pitch = max_pitch;
            if (pitch < -max_pitch)
                pitch = -max_pitch;
        }

    }

    void on_viewport_change(GLFWwindow* window, int w, int h) {
        width = w;
        height = h;
        glViewport(0, 0, width, height);
    }

    void on_key_press(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
            if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursor_disabled = false;

                // enable mouse capture by ImGUI
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursor_disabled = true;

                // disable mouse capture by ImGUI
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }
        }

        if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
            if (glfwGetWindowMonitor(window) == NULL) {
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 15360, 8640, GLFW_DONT_CARE);
            }
            else {
                glfwSetWindowMonitor(window, NULL, 100, 100, 800, 800, GLFW_DONT_CARE);
            }
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
        static bool was_disabled_last_frame = false;
        static glm::vec2 last_position;

        if (cursor_disabled) {
            const glm::vec2 current_position(xpos, ypos);

            if (was_disabled_last_frame) {
                const glm::vec2 diff = current_position - last_position;
                camera::update_pitch_yaw(-diff.y, diff.x);
            }

            last_position = current_position;
            was_disabled_last_frame = true;
        }
        else {
            was_disabled_last_frame = false;
        }
    }
}


int main(void)
{

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window::width, window::height, "Window Title", NULL, NULL);
    if (!window) return -2;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) return -3;

    printf("OpenGL Version %d.%d loaded\n", GLVersion.major, GLVersion.minor);

    glfwSetFramebufferSizeCallback(window, window::on_viewport_change);
    glfwSetCursorPosCallback(window, window::on_mouse_move);
    glfwSetKeyCallback(window, window::on_key_press);

    glViewport(0, 0, window::width, window::height);
    glEnable(GL_DEPTH_TEST);
    // IMGUI

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // EXAMPLE
    const GLuint vertices_dim = 8;
    GLfloat vertices[] = {
        -0.5, -0.5, -0.5,   0, 0, 1,    0.4f, 0.4f, 
        0.5, -0.5, -0.5,    0, 1, 0,    0.2f, 0.6f,  
        -0.5, -0.5, 0.5,    0, 1, 1,    0.0f, 0.8f, 
        0.5, -0.5, 0.5,     1, 0, 0,    0.2f, 0.8f,  

        -0.5, 0.5, -0.5,    1, 0, 0,    0.6f, 0.6f,  
        0.5, 0.5, -0.5,     1, 1, 0,    0.4f, 0.2f,
        -0.5, 0.5, 0.5,     1, 1, 1,    0.0f, 1.0f,
        0.5, 0.5, 0.5,      0, 0, 0,    0.2f, 1.0f,

    };

    GLuint indices[] = {
        2, 6, 7,
        2, 3, 7,
        3, 7, 1,
        5, 7, 1,
        6, 7, 4,
        5, 7, 4,
        2, 0, 6,
        0, 4, 6,
        2, 0, 3,
        1, 0, 3,
        0, 1, 4,
        1, 5, 4
    };

    stbi_set_flip_vertically_on_load(true);

    GLuint wall_texture_id;
    int wall_texture_width, wall_texture_height, wall_texture_nrChannels;
    {
        glGenTextures(1, &wall_texture_id);
        glBindTexture(GL_TEXTURE_2D, wall_texture_id);

        unsigned char* wall_texture_data = stbi_load("resources/wall.jpg", &wall_texture_width, &wall_texture_height, &wall_texture_nrChannels, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wall_texture_width, wall_texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, wall_texture_data);
        glGenerateTextureMipmap(wall_texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, NULL);

        stbi_image_free(wall_texture_data);
    }

    const GLuint N = IM_ARRAYSIZE(vertices) / vertices_dim;

    // VAO - Vertex Array Object
    // VBO - Vertex Buffer Object
    // EBO - Element Buffer Object
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertices_dim * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertices_dim * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertices_dim * sizeof(float), (void*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, NULL);

    GLuint shader_program = create_shader_program();
    glUseProgram(shader_program);

    // uniforms (global variables for shaders)
    GLuint uniform_factor = glGetUniformLocation(shader_program, "factor");
    GLuint uniform_rotation = glGetUniformLocation(shader_program, "rotation");
    GLuint uniform_transformation = glGetUniformLocation(shader_program, "transformation");

    // inputs
    float color_array[] { 0.7f, 0.1f, 0.2f };
    std::map<int, GLenum> polygon_modes_map{ {0, GL_FILL}, {1, GL_LINE}, {2, GL_POINT} };
    static int current_polygon_mode = 0;

    float dt = 0.0f;	// Time between current frame and last frame
    float last_time = 0.0f; // Time of last frame

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        printFPS();

        float current_time = static_cast<float>(glfwGetTime());
        dt = current_time - last_time;
        last_time = current_time;

        // IMGUI window
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float FOV = glm::pi<float>() / 2.f;

        {
            ImGui::Begin("App Config");

            ImGui::ColorEdit3("Background color", color_array);

            {
                static float factor = 0.5f;
                if (ImGui::SliderFloat("Factor", &factor, -2.f, 2.f)) {
                    glUniform1f(uniform_factor, factor);
                }
            }

            ImGui::SetNextTreeNodeOpen(true);
            if (ImGui::TreeNode("Camera")) {
                {
                    ImGui::SliderFloat("FOV", &FOV, 0, glm::pi<float>());
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
                    glm::mat4 projection = glm::perspective<float>(FOV, float(window::width) / window::height, 0.1f, 100.0f);

                    transformation = transformation * projection * view * model;
                    glUniformMatrix4fv(uniform_transformation, 1, GL_FALSE, glm::value_ptr(transformation));
                }

                ImGui::TreePop();
            }

            {
                const char * const labels[] = { "GL_FILL", "GL_LINE", "GL_POINT" };
                ImGui::Combo("Polygon mode", &current_polygon_mode, labels, IM_ARRAYSIZE(labels));
            }

            if (ImGui::TreeNode("Data")) {
                {
                    ImTextureID wall_texture = (void*)(intptr_t)wall_texture_id;
                    float my_tex_w = static_cast<float>(wall_texture_width);
                    float my_tex_h = static_cast<float>(wall_texture_height);
                    ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
                    ImGui::Image(wall_texture, ImVec2(my_tex_w, my_tex_h));
                }

                ImGui::TreePop();
            }

            ImGui::End();
        }

        /* Process events */
        window::camera::update_postition(window, dt);
        window::camera::update_look_at();

        /* Render here */
        glClearColor(color_array[0], color_array[1], color_array[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_texture_id);

        glBindVertexArray(VAO);

        glPolygonMode(GL_FRONT_AND_BACK, polygon_modes_map[current_polygon_mode]);
        glDrawElements(GL_TRIANGLES, IM_ARRAYSIZE(indices), GL_UNSIGNED_INT, NULL);


        {

            glm::vec3 cubePositions[] = {
                glm::vec3(2.0f,  5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f,  3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f,  2.0f, -2.5f),
                glm::vec3(1.5f,  0.2f, -1.5f),
                glm::vec3(-1.3f,  1.0f, -1.5f)
            };

            for (unsigned int i = 0; i < 9; i++)
            {
                glm::mat4 transformation = glm::mat4(1.0f);
                glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::pi<float>() * i * 0.1f, glm::vec3(1.0f, 0.3f, 0.5f));

                glm::mat4 view = glm::mat4(1.0f);
                view = view * glm::lookAt(window::camera::position, window::camera::look_at + window::camera::position, window::camera::up);
                view = glm::translate(view, cubePositions[i]);

                glm::mat4 projection = glm::perspective<float>(FOV, float(window::width) / window::height, 0.1f, 500.f);

                transformation = transformation * projection * view * model;
                glUniformMatrix4fv(uniform_transformation, 1, GL_FALSE, glm::value_ptr(transformation));

                glDrawElements(GL_TRIANGLES, IM_ARRAYSIZE(indices), GL_UNSIGNED_INT, NULL);
            }
        }

        glBindVertexArray(0);


        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
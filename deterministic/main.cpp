#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <math.h>

#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "main.hpp"
#include "utils_gl.hpp"
#include "InteractiveWindow.hpp"
#include "Texture.hpp"


int main(void)
{
    std::shared_ptr<AppData> app_data = std::make_shared<AppData>();

    GLFWwindow* window;
    window::app_data = app_data;

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

    window::initialize(window);

    // IMGUI
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    app_data->textures.wall = Texture::load_from_path("resources/wall.jpg");
    models::Cube cube;

    GLuint shader_program = create_shader_program();
    glUseProgram(shader_program);

    // uniforms (global variables for shaders)
    app_data->uniforms.factor = glGetUniformLocation(shader_program, "factor");
    app_data->uniforms.transformation = glGetUniformLocation(shader_program, "transformation");
    

    // Time between current frame and last frame
    float dt = 0.0f;	
    // Time of last frame
    float last_time = 0.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float current_time = static_cast<float>(glfwGetTime());
        dt = current_time - last_time;
        last_time = current_time;
        printFPS();


        glfwPollEvents();
        // IMGUI window
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        app_data_window();

        /* Process events */
        window::camera::update_postition(window, dt);
        window::camera::update_look_at();

        /* Render here */
        const glm::vec3& bg = app_data->bg_color;
        glClearColor(bg.x, bg.y, bg.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, app_data->polygon_modes_map[app_data->current_polygon_mode]);

        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app_data->textures.wall.id);

        cube.bind();
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

                glm::mat4 projection = glm::perspective<float>(window::camera::FOV, float(window::width) / window::height, 0.1f, 500.f);

                transformation = projection * view * model;
                glUniformMatrix4fv(app_data->uniforms.transformation, 1, GL_FALSE, glm::value_ptr(transformation));

                cube.draw();
            }
        }
        cube.unbind();


        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glDeleteProgram(shader_program);

    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
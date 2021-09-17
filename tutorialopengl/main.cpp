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

#include <CameraWindow.hpp>
#include "main.hpp"
#include "utils_gl.hpp"
#include "Texture.hpp"

void init_imgui(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void terminate_imgui(GLFWwindow* window) {
    ImGui::DestroyContext();
}

void start_frame(GLFWwindow* window) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void end_frame(GLFWwindow* window) {
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

namespace transform {

    glm::mat4 projection(const CameraWindow& window) {
        return glm::perspective<float>(window.camera.FOV, float(window.window.width) / window.window.height, 0.1f, 500.f);
    }

    glm::mat4 view(const CameraWindow& window) {
        return glm::lookAt(window.camera.position, window.camera.look_at + window.camera.position, window.camera.up);
    }
}


float calculate_dt() {
    static float last_time = static_cast<float>(glfwGetTime());
    float current_time = static_cast<float>(glfwGetTime());
    float dt = current_time - last_time;
    last_time = current_time;
    return dt;
}

int main(void)
{
    std::shared_ptr<AppData> app_data = std::make_shared<AppData>();

    /* Initialize the library */
    //glfwWindowHint(GLFW_SAMPLES, 16);
    SimpleWindow::init_glfw();

    /* Create a windowed mode window and its OpenGL context */
    CameraWindow window("Test 01");

    bool glad_loaded = gladLoadGL();
    assert(glad_loaded);

    printf("OpenGL Version %d.%d loaded\n", GLVersion.major, GLVersion.minor);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    init_imgui(window.window.window_ptr);

    app_data->textures.wall = Texture::load_from_path("resources/wall.jpg");
    app_data->textures.crate = CubeTexture::load_from_path("resources/crate.png");
    models::Cube cube;

    models::Cube white_cube;
    for (int i = 0; i < white_cube.vertices.size(); ++i) {
        int index = i % white_cube.vertex_size();
        if (index > 2 && index < 6)
            white_cube.vertices[i] = 1.0f;
    }
    white_cube.update_buffers();

    GLuint shader_program = create_shader_program();
    glUseProgram(shader_program);

    // init uniforms (global variables for shaders)
    app_data->init_uniforms(shader_program);

    struct {
        GLuint camera_position;
    } local_uniforms;

    local_uniforms.camera_position = glGetUniformLocation(shader_program, "camera_position");

    // Time between current frame and last frame
    float dt = 0.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.window.window_ptr))
    {
        dt = calculate_dt();

        start_frame(window.window.window_ptr);

        window.update_camera_postition(dt);
        window.camera.update_look_at();

        glUniform3fv(local_uniforms.camera_position, 1, glm::value_ptr(window.camera.position));

        {
            ImGui::Begin("Window configuration");
            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
            if (ImGui::TreeNode("Display mode")) {
                ImGui::Text("[F11] - enter/exit fullscreen mode");
                ImGui::Spacing();
                window.window.imgui_panel();
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Camera")) {
                ImGui::Text("        [F1] - enter/exit camera mode");
                ImGui::Text("[W][A][S][D] - move around the scene");
                ImGui::Spacing();
                window.camera.imgui_panel();
                ImGui::TreePop();
            }
            ImGui::End();
        }

        {
            ImGui::Begin("Application data");
            app_data->imgui_panel();
            ImGui::End();
        }

        /* Render here */
        const glm::vec3& bg = app_data->bg_color;
        glClearColor(bg.x, bg.y, bg.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, app_data->polygon_modes_map[app_data->current_polygon_mode]);

        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app_data->textures.wall.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, app_data->textures.crate.id);

        // group of cubes
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

            const FirstPersonCamera& cam = window.camera;
            cube.bind();
            for (unsigned int i = 0; i < 9; i++)
            {

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                //float rot = 0.1f;
                model = glm::rotate(model, glm::pi<float>() * i * app_data->rot, glm::vec3(1.0f, 0.3f, 0.5f));

                glm::mat4 view = transform::view(window);
                glm::mat4 projection = transform::projection(window);
                glUniformMatrix4fv(app_data->uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(app_data->uniforms.view, 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(app_data->uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
                cube.draw();
            }
            cube.unbind();
        }
        // light source cube
        white_cube.bind();
        {
            glm::mat4 model = glm::mat4(1.0f); 
            model = glm::translate(model, app_data->light_position);
            model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

            glm::mat4 view = transform::view(window);
            glm::mat4 projection = transform::projection(window);
            glUniformMatrix4fv(app_data->uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(app_data->uniforms.view, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(app_data->uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
            white_cube.draw();
        }
        white_cube.unbind();

        end_frame(window.window.window_ptr);

        if(GLenum e = glGetError())
            std::cout << std::hex << e << std::endl;
    }

    glDeleteProgram(shader_program);

    terminate_imgui(window.window.window_ptr);

    return 0;
}
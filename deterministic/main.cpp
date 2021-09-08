#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
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

#include "InteractiveWindow.hpp"

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

    window::initialize(window);

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
         0.5, -0.5, -0.5,   0, 1, 0,    0.2f, 0.6f,  
        -0.5, -0.5,  0.5,   0, 1, 1,    0.0f, 0.8f, 
         0.5, -0.5,  0.5,   1, 0, 0,    0.2f, 0.8f,  

        -0.5,  0.5, -0.5,   1, 0, 0,    0.6f, 0.6f,  
         0.5,  0.5, -0.5,   1, 1, 0,    0.4f, 0.2f,
        -0.5,  0.5,  0.5,   1, 1, 1,    0.0f, 1.0f,
         0.5,  0.5,  0.5,   0, 0, 0,    0.2f, 1.0f,

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

        glBindTexture(GL_TEXTURE_2D, 0);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint shader_program = create_shader_program();
    glUseProgram(shader_program);

    // uniforms (global variables for shaders)
    window::uniform_factor = glGetUniformLocation(shader_program, "factor");
    window::uniform_rotation = glGetUniformLocation(shader_program, "rotation");
    window::uniform_transformation = glGetUniformLocation(shader_program, "transformation");
    

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

        app_config_window();

        /* Process events */
        window::camera::update_postition(window, dt);
        window::camera::update_look_at();

        /* Render here */
        glClearColor(window::color_array[0], window::color_array[1], window::color_array[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_texture_id);

        glBindVertexArray(VAO);

        glPolygonMode(GL_FRONT_AND_BACK, window::polygon_modes_map[window::current_polygon_mode]);
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

                glm::mat4 projection = glm::perspective<float>(window::camera::FOV, float(window::width) / window::height, 0.1f, 500.f);

                transformation = transformation * projection * view * model;
                glUniformMatrix4fv(window::uniform_transformation, 1, GL_FALSE, glm::value_ptr(transformation));

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
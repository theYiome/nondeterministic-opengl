#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "utils_gl.hpp"


void on_viewport_change(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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
    const int window_width = 900;
    const int window_height = 900;
    window = glfwCreateWindow(window_width, window_height, "Window Title", NULL, NULL);
    if (!window) return -2;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) return -3;

    printf("OpenGL Version %d.%d loaded\n", GLVersion.major, GLVersion.minor);

    glfwSetFramebufferSizeCallback(window, on_viewport_change);
    glViewport(0, 0, window_width, window_height);

    // IMGUI

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // EXAMPLE
    const GLuint vertices_dim = 3;
    GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    const GLuint N = sizeof(vertices) / vertices_dim / sizeof(GL_FLOAT);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint shader_program = create_shader_program();
    glUseProgram(shader_program);


    float color_array[] { 0.7f, 0.1f, 0.2f };

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // IMGUI window
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("App Config");
            ImGui::ColorEdit3("Background color", color_array);
            ImGui::End();
        }


        /* Render here */
        glClearColor(color_array[0], color_array[1], color_array[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
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
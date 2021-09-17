#pragma once
#include <vector>
#include <numeric>

#include <GLAD/glad.h>
#include <glm/glm.hpp>

#include "Texture.hpp"

struct AppData {
    glm::vec3 bg_color = glm::vec3(0.f, 0.f, 0.f);
   
    std::map<int, GLenum> polygon_modes_map { 
        {0, GL_FILL}, {1, GL_LINE}, {2, GL_POINT} 
    };
    int current_polygon_mode = 0;

    float color_ratio = 1.0f;
    float light_strength = 4.f;
    float ambient_strength = 0.1f;
    float specular_strength = 0.5f;
    float specular_power = 8.f;
    float rot = 0.1f;
    glm::vec3 light_position = glm::vec3(0.f, 1.f, -5.f);
    glm::vec3 light_color = glm::vec3(1.f, 0.4f, 0.3f);

    struct {
        GLint model;
        GLint view;
        GLint projection;

        GLint color_ratio;

        GLint light_strength;
        GLint light_position;
        GLint light_color;
        GLint ambient_strength;
        GLint specular_strength;
        GLint specular_power;
    } uniforms;

    void init_uniforms(GLuint shader_program) {
        uniforms.model = glGetUniformLocation(shader_program, "model");
        uniforms.view = glGetUniformLocation(shader_program, "view");
        uniforms.projection = glGetUniformLocation(shader_program, "projection");
        uniforms.color_ratio = glGetUniformLocation(shader_program, "color_ratio");

        uniforms.light_strength = glGetUniformLocation(shader_program, "light_strength");
        uniforms.light_position = glGetUniformLocation(shader_program, "light_position");
        uniforms.light_color = glGetUniformLocation(shader_program, "light_color");
        uniforms.ambient_strength = glGetUniformLocation(shader_program, "ambient_strength");
        uniforms.specular_strength = glGetUniformLocation(shader_program, "specular_strength");
        uniforms.specular_power = glGetUniformLocation(shader_program, "specular_power");
        //assert(uniforms.color_ratio != -1);
        //assert(uniforms.transformation != -1);
        //assert(uniforms.light_position != -1);
        //assert(uniforms.light_color != -1);
    }

    struct {
        Texture wall;
        CubeTexture crate;
    } textures;

    void imgui_panel() {
        ImGui::ColorEdit3("Background color", glm::value_ptr(bg_color));

        {
            const char* const labels[] = { "GL_FILL", "GL_LINE", "GL_POINT" };
            ImGui::Combo("Drawing mode", &current_polygon_mode, labels, IM_ARRAYSIZE(labels));
        }

        ImGui::SliderFloat("Rotation", &rot, 0.f, 1.f);

        if (ImGui::SliderFloat("Texture -> Vertex", &color_ratio, 0.f, 1.f)) {
            glUniform1f(uniforms.color_ratio, color_ratio);
        }

        if (ImGui::DragFloat("Light strength", &light_strength, 0.01f)) {
            glUniform1f(uniforms.light_strength, light_strength);
        }

        if (ImGui::SliderFloat("Ambient strength", &ambient_strength, 0.f, 1.f)) {
            glUniform1f(uniforms.ambient_strength, ambient_strength);
        }

        if (ImGui::SliderFloat("Specular strength", &specular_strength, 0.f, 1.f)) {
            glUniform1f(uniforms.specular_strength, specular_strength);
        }

        if (ImGui::SliderFloat("Specular power", &specular_power, 1.f, 128.f)) {
            glUniform1f(uniforms.specular_power, specular_power);
        }

        if (ImGui::DragFloat3("Light position", glm::value_ptr(light_position), 0.01f))
            glUniform3fv(uniforms.light_position, 1, glm::value_ptr(light_position));
            
        if (ImGui::ColorEdit3("Light color", glm::value_ptr(light_color)))
            glUniform3fv(uniforms.light_color, 1, glm::value_ptr(light_color));

        //ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Textures")) {
            {
                const Texture& tex = textures.wall;
                ImTextureID imgui_texture = (void*)(intptr_t)tex.id;
                ImGui::Text("%d x %d", tex.width, tex.height);
                ImGui::Image(imgui_texture, ImVec2(tex.width / 2, tex.height / 2));
            }

            {
                const CubeTexture& tex = textures.crate;
                ImTextureID imgui_texture = (void*)(intptr_t)tex.id;
                ImGui::Text("%d x %d", tex.width, tex.height);
                ImGui::Image(imgui_texture, ImVec2(tex.width / 2, tex.height / 2));
            }
            ImGui::TreePop();
        }
    }
};


namespace models {
    struct Cube {
        std::vector<GLuint> indices = {
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

        GLuint indices_bytesize() {
            return indices.size() * sizeof(GLuint);
        }

        std::vector <GLfloat> vertices = {
            -0.5, -0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.4f, 0.4f,     -0.5, -0.5, -0.5,
             0.5, -0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.2f, 0.6f,      0.5, -0.5, -0.5,
            -0.5, -0.5,  0.5,   1.0f, 0.5f, 0.31f,  0.0f, 0.8f,     -0.5, -0.5,  0.5,
             0.5, -0.5,  0.5,   0.7f, 0.2f, 0.90f,  0.2f, 0.8f,      0.5, -0.5,  0.5,

            -0.5,  0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.6f, 0.6f,     -0.5,  0.5, -0.5,
             0.5,  0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.4f, 0.2f,      0.5,  0.5, -0.5,
            -0.5,  0.5,  0.5,   1.0f, 0.5f, 0.31f,  0.0f, 1.0f,     -0.5,  0.5,  0.5,
             0.5,  0.5,  0.5,   0.7f, 0.2f, 0.90f,  0.2f, 1.0f,      0.5,  0.5,  0.5,
        };

        GLuint verticies_bytesize() {
            return vertices.size() * sizeof(GLfloat);
        }

        std::vector <GLuint> vertex_attributes = { 3, 3, 2, 3};

        GLuint vertex_size() {
            return std::accumulate(vertex_attributes.begin(), vertex_attributes.end(), 0);
        }

        GLuint vertex_bytesize() {
            return vertex_size() * sizeof(GL_FLOAT);
        }

        const GLuint vertices_dim = 8;
        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;

        void clear() {
            if (VAO) {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
                VAO = VBO = EBO = 0;
            }
        }

        void create() {
            clear();

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            bind();
            GLuint offset = 0;
            for (GLuint i = 0; i < vertex_attributes.size(); ++i) {
                glVertexAttribPointer(i, vertex_attributes[i], GL_FLOAT, GL_FALSE, vertex_bytesize(), (void*)(offset * sizeof(GL_FLOAT)));
                glEnableVertexAttribArray(i);
                offset += vertex_attributes[i];
            }
            upload_data();
            unbind();
        }

        void update_buffers() {
            bind();
            upload_data();
            unbind();
        }

        Cube() {
            create();
        }

        ~Cube() {
            clear();
        }

        void bind() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        }

        void unbind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void draw() {
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
        }

    private:
        void upload_data() {
            glBufferData(GL_ARRAY_BUFFER, verticies_bytesize(), vertices.data(), GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_bytesize(), indices.data(), GL_DYNAMIC_DRAW);
        }
    };
}
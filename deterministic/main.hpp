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

    struct {
        GLuint factor;
        GLuint transformation;
    } uniforms;

    struct {
        Texture wall;
    } textures;
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
            -0.5, -0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.4f, 0.4f,
             0.5, -0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.2f, 0.6f,
            -0.5, -0.5,  0.5,   1.0f, 0.5f, 0.31f,  0.0f, 0.8f,
             0.5, -0.5,  0.5,   0.7f, 0.2f, 0.90f,  0.2f, 0.8f,

            -0.5,  0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.6f, 0.6f,
             0.5,  0.5, -0.5,   1.0f, 0.5f, 0.31f,  0.4f, 0.2f,
            -0.5,  0.5,  0.5,   1.0f, 0.5f, 0.31f,  0.0f, 1.0f,
             0.5,  0.5,  0.5,   0.7f, 0.2f, 0.90f,  0.2f, 1.0f
        };

        GLuint verticies_bytesize() {
            return vertices.size() * sizeof(GLfloat);
        }

        std::vector <GLuint> vertex_attributes = { 3, 3, 2 };

        GLuint vertex_size() {
            return std::accumulate(vertex_attributes.begin(), vertex_attributes.end(), 0);
        }

        GLuint vertex_bytesize() {
            return vertex_size() * sizeof(GL_FLOAT);
        }

        const GLuint vertices_dim = 8;
        GLuint vao_id;
        GLuint vbo_id;
        GLuint ebo_id;

        Cube() {
            glGenVertexArrays(1, &vao_id);
            glGenBuffers(1, &vbo_id);
            glGenBuffers(1, &ebo_id);

            glBindVertexArray(vao_id);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBufferData(GL_ARRAY_BUFFER, verticies_bytesize(), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_bytesize(), indices.data(), GL_STATIC_DRAW);

            for (GLuint i = 0; i < vertex_attributes.size(); ++i) {
                static GLuint offset = 0;
                
                glVertexAttribPointer(i, vertex_attributes[i], GL_FLOAT, GL_FALSE, vertex_bytesize(), (void*)(offset * sizeof(GL_FLOAT)));
                glEnableVertexAttribArray(i);

                offset += vertex_attributes[i];
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ~Cube() {
            glDeleteVertexArrays(1, &vao_id);
            glDeleteBuffers(1, &vbo_id);
            glDeleteBuffers(1, &ebo_id);
        }

        void bind() {
            glBindBuffer(GL_ARRAY_BUFFER, vao_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
        }

        void unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void draw() {
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
        }
    };
}
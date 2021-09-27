#pragma once
#include <vector>
#include <numeric>

#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
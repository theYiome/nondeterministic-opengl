#pragma once
#include <map>

#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.hpp"

namespace examples {
    namespace basic_light {
        struct Data {
            glm::vec3 bg_color = glm::vec3(0.f, 0.f, 0.f);

            std::map<int, GLenum> polygon_modes_map{
                {0, GL_FILL}, {1, GL_LINE}, {2, GL_POINT}
            };
            int current_polygon_mode = 0;

            float color_ratio = 0.f;
            float light_strength = 4.f;
            float ambient_strength = 0.1f;
            float specular_strength = 0.5f;
            float specular_power = 8.f;
            float rot = 0.1f;
            glm::vec3 light_position = glm::vec3(0.f, 1.f, -5.f);
            glm::vec3 light_color = glm::vec3(1.f, 1.f, 1.f);

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

            struct {
                Texture wall;
                CubeTexture crate;
            } textures;

            void init_uniforms(GLuint shader_program);

            void imgui_panel();
        };

        void run();
    }

    namespace rt_spheres {
        struct Data {

        };

        void run();
    }
}
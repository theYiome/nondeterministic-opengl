#include <cassert>
#include <tuple>

#include <GLAD/glad.h>

#include <imgui.h>

#include "Models.hpp"
#include "utils_gl.hpp"
#include "Texture.hpp"
#include "utils.hpp"
#include "imgui_utils.hpp"

#include "examples.hpp"

namespace examples {
    namespace basic_light {
        
        void Data::init_uniforms(GLuint shader_program) {
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
        }

        void Data::imgui_panel() {
            ImGui::ColorEdit3("Background color", glm::value_ptr(bg_color));

            // TODO use new combo API, update mode only on change
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
                ImGui::TreePop();
            }
        }

	    void run() {
            examples::basic_light::Data app;
            Data app_data;

            /* Initialize the library */
            //SimpleWindow::init_glfw();

            /* Create a windowed mode window and its OpenGL context */
            CameraWindow camera_window("Cubes and a light source");

            app_data.textures.wall = Texture::load_from_path("resources/wall.jpg");
            app_data.textures.crate = CubeTexture::load_from_path("resources/crate.png");
            models::Cube cube;

            models::Cube white_cube;
            for (int i = 0; i < white_cube.vertices.size(); ++i) {
                int index = i % white_cube.vertex_size();
                if (index > 2 && index < 6)
                    white_cube.vertices[i] = 1.0f;
            }
            white_cube.update_buffers();

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_MULTISAMPLE);
            glfwWindowHint(GLFW_SAMPLES, 4);

            GLuint shader_program = create_shader_program("resources/light_vert.glsl", "resources/light_frag.glsl");
            glUseProgram(shader_program);

            // init uniforms (global variables for shaders)
            app_data.init_uniforms(shader_program);

            struct {
                GLuint camera_position;
            } local_uniforms;

            local_uniforms.camera_position = glGetUniformLocation(shader_program, "camera_position");

            // Time between current frame and last frame
            float dt = 0.0f;

            /* Loop until the user closes the window */
            while (!camera_window.window.should_close())
            {
                dt = calculate_dt();

                camera_window.window.start_frame();

                camera_window.update_camera_postition(dt);
                camera_window.camera.update_look_at();

                imgui_utils::render(camera_window);
                imgui_utils::render(app_data);

                glUniform3fv(local_uniforms.camera_position, 1, glm::value_ptr(camera_window.camera.position));

                /* Render here */
                glClearColor(app_data.bg_color.x, app_data.bg_color.y, app_data.bg_color.z, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPolygonMode(GL_FRONT_AND_BACK, app_data.polygon_modes_map[app_data.current_polygon_mode]);

                glUseProgram(shader_program);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, app_data.textures.crate.id);

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

                    const FirstPersonCamera& cam = camera_window.camera;
                    cube.bind();
                    for (unsigned int i = 0; i < 9; i++)
                    {

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, cubePositions[i]);
                        //float rot = 0.1f;
                        model = glm::rotate(model, glm::pi<float>() * i * app_data.rot, glm::vec3(1.0f, 0.3f, 0.5f));
                        model = glm::scale(model, glm::vec3(1.f - 0.05f * i, 0.6f + 0.05f * i, 1.f));

                        glm::mat4 view = transform::view(camera_window);
                        glm::mat4 projection = transform::projection(camera_window);
                        glUniformMatrix4fv(app_data.uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
                        glUniformMatrix4fv(app_data.uniforms.view, 1, GL_FALSE, glm::value_ptr(view));
                        glUniformMatrix4fv(app_data.uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
                        cube.draw();
                    }
                    cube.unbind();
                }
                // light source cube
                white_cube.bind();
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, app_data.light_position);
                    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

                    glm::mat4 view = transform::view(camera_window);
                    glm::mat4 projection = transform::projection(camera_window);
                    glUniformMatrix4fv(app_data.uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
                    glUniformMatrix4fv(app_data.uniforms.view, 1, GL_FALSE, glm::value_ptr(view));
                    glUniformMatrix4fv(app_data.uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
                    white_cube.draw();
                }
                white_cube.unbind();

                camera_window.window.end_frame();

                if (GLenum e = glGetError())
                    std::cout << "glGetError() = " << std::hex << e << std::endl;
            }

            glDeleteProgram(shader_program);
	    }
    }

    namespace rt_spheres {

        static std::tuple<GLuint, GLuint, GLuint> create_rectangle(const GLfloat& size = 0.2f) {
            GLuint VAO, VBO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
            glEnableVertexAttribArray(1);

            GLfloat vertices[] = {
                -size,  -size,     0.f, 0.f,
                -size,   size,     0.f, 1.f,
                 size,   size,     1.f, 1.f,
                 size,  -size,     1.f, 0.f,
            };

            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            GLuint indicies[] = {
                0, 1, 2,
                0, 3, 2
            };

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

            // https://gamedev.stackexchange.com/questions/90471/should-unbind-buffer
            // Remember to unbind the VAO first.
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            return { VAO, VBO, EBO };
        }

        struct Pixel {
            GLfloat r;
            GLfloat g;
            GLfloat b;
        };

        struct FrameBuffer {
            GLuint texture_id;

            GLuint width;
            GLuint height;
            std::vector<Pixel> data;

            Pixel& at(const GLuint& x, const GLuint& y) {
                assert(x < width);
                assert(y < height);
                return data[x * width + y];
            }

            GLfloat* raw_data() {
                return reinterpret_cast<GLfloat*>(data.data());
            }

            void update() {
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, raw_data());
                glGenerateTextureMipmap(texture_id);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            void allocate(GLuint new_width, GLuint new_height) {
                width = new_width;
                height = new_height;
                data.resize(width * height);
            }

            FrameBuffer() : width(0), height(0) {
                glGenTextures(1, &texture_id);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            void bind() {
                glBindTexture(GL_TEXTURE_2D, texture_id);
            }

            void unbind() {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        };

        void render(FrameBuffer& buffer, GLuint w, GLuint h) {
            if (w != buffer.width || h != buffer.height)
                buffer.allocate(w, h);

            #pragma omp parallel for
            for (int x = 0; x < w; ++x)
                for (int y = 0; y < h; ++y) {
                    GLuint index = x + y * w;
                    assert(index < buffer.data.size());

                    GLfloat a = x / GLfloat(w);
                    GLfloat b = y / GLfloat(h);

                    buffer.data[index] = {1.f, a, b};
                }

            buffer.update();
        }

        void run() {
            /* Create a windowed mode window and its OpenGL context */
            CameraWindow camera_window("RT Spheres");

            GLuint shader_program = create_shader_program("resources/rt_vert.glsl", "resources/rt_frag.glsl");

            FrameBuffer buffer;

            // Time between current frame and last frame
            float dt = 0.0f;

            auto [VAO, VBO, EBO] = create_rectangle(1.f);

            /* Loop until the user closes the window */
            while (!camera_window.window.should_close())
            {
                printFPS();
                dt = calculate_dt();

                camera_window.window.start_frame();

                camera_window.update_camera_postition(dt);
                camera_window.camera.update_look_at();

                imgui_utils::render(camera_window);

                /* update texture */
                GLuint w = camera_window.window.width;
                GLuint h = camera_window.window.height;

                render(buffer, w, h);

                /* Render here */
                glClearColor(0.f, 0.f, 0.f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glUseProgram(shader_program);

                glBindVertexArray(VAO);
                glActiveTexture(GL_TEXTURE0);
                buffer.bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                buffer.unbind();
                glBindVertexArray(0);

                camera_window.window.end_frame();

                if (GLenum e = glGetError())
                    std::cout << "glGetError() = " << std::hex << e << std::endl;
            }

            glDeleteProgram(shader_program);
        }
    }
}
#pragma once
#include <cassert>
#include <cmath>
#include <tuple>

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include "Models.hpp"
#include "utils_gl.hpp"
#include "Texture.hpp"
#include "utils.hpp"
#include "imgui_utils.hpp"

#include "rt_primitives.hpp"

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
            }

            void imgui_panel() {
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
        };

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

                ImGui::Begin("Application data");
                app_data.imgui_panel();
                ImGui::End();

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

        struct FrameBuffer {
            GLuint texture_id;

            GLuint width;
            GLuint height;
            std::vector<Pixel> data;

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
                bind();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                unbind();
            }

            void bind() {
                glBindTexture(GL_TEXTURE_2D, texture_id);
            }

            void unbind() {
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            void set_filtering(GLenum filtering) {
                bind();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
                unbind();
            }
        };

        struct Scene {
            const FirstPersonCamera& cam;
            std::vector<Sphere> spheres;
            std::vector<Plane> planes;
            std::vector<Light> lights;
            Pixel ambient = { 0.2f, 0.2f, 0.2f };

            Scene(const FirstPersonCamera& camera): cam(camera) {
                spheres.push_back({ {0.f, 0.f, -1.f}, 1.f});
                spheres[0].material.relfectivity = 0.8f;
                spheres.push_back({ {1.f, 1.f, 1.f}, 0.3f, {0.f, 0.f, 1.f} });
                spheres.push_back({ {1.f, 5.f, 3.f}, 0.8f, {0.f, 1.f, 1.f} });
                spheres.push_back({ {-3.f, 1.f, 1.f}, 1.3f, {1.f, 0.f, 1.f} });
                spheres.push_back({ {2.f, -1.f, -1.f}, 0.1f, {0.f, 1.f, 0.f} });

                planes.push_back({ {0.f, -1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.25f, 0.f} });

                lights.push_back({ { 0.f, 10.f, 0.f }, { 1.f, 1.f, 1.f }, 1.f });
                lights.push_back({ { 3.f, 3.f, 3.f }, { 0.9f, 0.2f, 0.3f }, 1.f });
                lights.push_back({ { -6.f, 5.f, 10.f }, { 0.1f, 0.4f, 0.7f }, 1.f });
                lights.push_back({ { 6.f, 5.f, 2.f }, { 0.9f, 0.4f, 0.7f }, 1.f });
                lights.push_back({ { -6.f, 5.f, -5.f }, { 0.1f, 0.9f, 0.7f }, 1.f });
            }

            void imgui_panel() {
                ImGui::Begin("Scene");
                
                int i = 0;
                if (ImGui::TreeNode("Spheres")) {
                    for (Sphere& s : spheres) {
                        ImGui::PushID(i++);
                        ImGui::DragFloat3("Position", glm::value_ptr(s.position), 0.01f);
                        ImGui::DragFloat("Radius", &s.r, 0.01f, 0.1f);
                        s.material.imgui_panel();
                        ImGui::PopID();

                        ImGui::Spacing();
                    }
                    ImGui::TreePop();
                }

                i = 0;
                if (ImGui::TreeNode("Planes")) {
                    for (Plane& p : planes) {
                        ImGui::PushID(i++);
                        ImGui::DragFloat3("Position", glm::value_ptr(p.position), 0.01f);
                        ImGui::DragFloat3("Normal", glm::value_ptr(p.normal), 0.01f);
                        p.material.imgui_panel();
                        ImGui::PopID();

                        ImGui::Spacing();
                    }
                    ImGui::TreePop();
                }


                i = 0;
                if (ImGui::TreeNode("Lights")) {
                    Pixel new_ambient = { 0.f, 0.f, 0.f };
                    for (Light& l : lights) {
                        ImGui::PushID(i++);
                        ImGui::DragFloat3("Position", glm::value_ptr(l.position), 0.01f);
                        ImGui::ColorEdit3("Color", (float*)&l.color);
                        ImGui::DragFloat("Intensity", &l.intensity, 0.01f, 0.1f);
                        ImGui::PopID();
                        ImGui::Spacing();

                        new_ambient = new_ambient + l.color;
                    }
                    ImGui::TreePop();

                    if (lights.size() > 0)
                        ambient = new_ambient * (1.f / lights.size()) * 0.2f;
                }

                ImGui::ColorEdit3("Ambient light", (float*)&ambient);

                ImGui::End();
            }
        };

        Ray calculate_vieport_ray(const FirstPersonCamera& cam, const int& w, const int& h, const int& x, const int& y) {
            float d = 1.f / (cam.FOV + 0.1f);
            glm::vec3 vx = -glm::normalize(glm::cross(cam.up, cam.look_at));
            glm::vec3 vy = glm::normalize(glm::cross(vx, cam.look_at));

            glm::vec3 base = cam.position + cam.look_at * d;

            const float dv = 1.f / float(w);

            const float dx = float(x) - (float(w) / 2.f);
            const float dy = float(y) - (float(h) / 2.f);

            glm::vec3 final_point = base + (vx * dv * dx) + (vy * dv * dy);

            glm::vec3 direction = glm::normalize(final_point - cam.position);

            return { cam.position, direction };
        }
    
        inline std::tuple<GLfloat, GLfloat, Material, glm::vec3> closest_collision(const Ray& ray, const Scene& scene) {
            GLfloat closest_distance = f32inf;
            GLfloat closest_distance2 = f32inf;
            Material closest_material = { {0.f, 0.f, 0.f} };
            glm::vec3 normal;

            for (const Sphere& s : scene.spheres) {
                auto [current_distance, current_distance2] = s.intersects2(ray);
                if (current_distance < closest_distance) {
                    closest_material = s.material;
                    closest_distance = current_distance;
                    closest_distance2 = current_distance2;
                    normal = glm::normalize(ray.at(current_distance) - s.position);
                }
            }

            for (const Light& l : scene.lights) {
                auto [current_distance, current_distance2] = l.intersects2(ray);
                if (current_distance < closest_distance) {
                    closest_material = { l.color, 1.0f, 0.f };
                    closest_distance = current_distance;
                    closest_distance2 = current_distance2;
                }
            }

            for (const Plane& p : scene.planes) {
                GLfloat current_distance = p.intersects(ray);
                if (current_distance < closest_distance) {
                    closest_material = p.material;
                    closest_distance = current_distance;
                    closest_distance2 = current_distance;
                    normal = glm::normalize(p.normal);
                }
            }

            constexpr float SELF_COLLISION_HACK_FRONT = 0.99999f;
            constexpr float SELF_COLLISION_HACK_BACK = 2.f - SELF_COLLISION_HACK_FRONT;
            return { closest_distance * SELF_COLLISION_HACK_FRONT, closest_distance2 * SELF_COLLISION_HACK_BACK, closest_material, normal };
        }

        float calculate_light_attenuation(const glm::vec3 primitive_normal, const glm::vec3 ray_direction, const float& distance) {
            float factor = glm::dot(ray_direction, primitive_normal);
            //factor *= 100.f / (distance * distance);
            constexpr float offset = 0.0f;
            factor = factor < offset ? offset : factor;
            factor -= offset;
            return factor;
        }

        Pixel light_sum(const glm::vec3& pixel_position, const glm::vec3& normal, const Material& material, const Scene& scene) {
            Pixel sum = material.color * scene.ambient;
            for (const Light& l : scene.lights) {
                Ray r = { pixel_position, glm::normalize(l.position - pixel_position) };
                auto [d, d2, m, n] = closest_collision(r, scene);

                if (m.emissivity > 0.f) {
                    float attenuation = calculate_light_attenuation(normal, r.direction, d);
                    sum = sum + material.color * l.color * attenuation;
                }
            }

            return sum;
        }

        Pixel recursive_tracing(int traces, const Material& material, const Ray& ray, const glm::vec3& normal, const float& distance, const float& distance2, const Scene& scene) {
            glm::vec3 pixel_position = ray.at(distance);
            Pixel sum = light_sum(pixel_position, normal, material, scene);

            if (traces <= 0) return sum;
            if (material.relfectivity == 0.f && material.transparency == 0.f) return sum;

            Pixel reflective_part = { 0.f, 0.f, 0.f };
            if (material.relfectivity > 0.f) {
                Ray r = { pixel_position, ray.direction - normal * 2.f * glm::dot(ray.direction, normal) };
                auto [d, d2, m, n] = closest_collision(r, scene);
                reflective_part = recursive_tracing(traces - 1, m, r, n, d, d2, scene);
            }

            Pixel transparent_part = { 0.f, 0.f, 0.f };
            if (material.transparency != 0.f) {
                Ray r = { ray.at(distance2), ray.direction };

                if (material.diffraction > 0.f)
                    r.direction = ray.direction + (normal * material.diffraction);

                auto [d, d2, m, n] = closest_collision(r, scene);
                transparent_part = recursive_tracing(traces - 1, m, r, n, d, d2, scene);
            }

            float complement = 1.f - material.relfectivity - material.transparency;
            complement = complement < 0.f ? 0.f : complement;

            return (sum * complement) + (reflective_part * material.relfectivity) + (transparent_part * material.transparency);
        }

        struct RayTracingSettings {
            int max_bounces = 2;
        };

        inline void kernel(std::vector<Pixel>& pixels, const int& x, const int& y, const GLuint& w, const GLuint& h, const Scene& scene, const RayTracingSettings& settings) {
            GLuint index = x + y * w;
            assert(index < pixels.size());

            Ray ray = calculate_vieport_ray(scene.cam, w, h, x, y);

            auto [distance, distance2, material, normal] = closest_collision(ray, scene);

            if (material.emissivity > 0.f) {
                pixels[index] = material.color * material.emissivity;
                return;
            }
            
            if (distance == f32inf) {
                pixels[index] = { 0.f, 0.f, 0.f };
                return;
            }

            pixels[index] = recursive_tracing(settings.max_bounces, material, ray, normal, distance, distance2, scene);
        }

        void render(FrameBuffer& buffer, GLuint w, GLuint h, const Scene& scene, const RayTracingSettings& settings) {
            if (w != buffer.width || h != buffer.height)
                buffer.allocate(w, h);

            #pragma omp parallel for
            for (int x = 0; x < w; ++x)
                for (int y = 0; y < h; ++y) {
                    kernel(buffer.data, x, y, w, h, scene, settings);
                }

            buffer.update();
        }

        void run() {
            /* Create a windowed mode window and its OpenGL context */
            CameraWindow camera_window("RT Spheres");
            Scene scene(camera_window.camera);
            RayTracingSettings settings;

            GLuint shader_program = create_shader_program("resources/rt_vert.glsl", "resources/rt_frag.glsl");

            FrameBuffer buffer;
            buffer.set_filtering(GL_LINEAR);


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
                scene.imgui_panel();

                // RT settings and rendering
                static float factor = 2.5f;
                {
                    ImGui::Begin("RT Settings");
                    ImGui::DragFloat("Decrease resolution", &factor, 0.01f, 0.8f, 100.f);
                    //factor = factor < 0.8f ? 0.8f : factor;
                    ImGui::InputInt("Maximum bounces", &settings.max_bounces);
                    ImGui::End();
                }

                GLuint w = camera_window.window.width;
                GLuint h = camera_window.window.height;

                /* update texture */
                render(buffer, w / factor, h / factor, scene, settings);

                /* Render here */
                glClearColor(0.f, 0.f, 0.f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glUseProgram(shader_program);

                glBindVertexArray(VAO);
                glActiveTexture(GL_TEXTURE0);
                //buffer.set_filtering(GL_LINEAR);
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
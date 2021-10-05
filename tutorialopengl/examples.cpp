#include <cassert>
#include <cmath>
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

        constexpr GLfloat f32inf = std::numeric_limits<GLfloat>::infinity();

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

        template <typename T>
        struct PixelTemplate {
            GLfloat r;
            GLfloat g;
            GLfloat b;

            PixelTemplate operator*(const float& m) {
                return { r * m, g * m, b * m };
            }

            PixelTemplate operator+(PixelTemplate& pixel2) {
                return { r + pixel2.r, g + pixel2.g, b + pixel2.b };
            }
        };
        typedef PixelTemplate<GLfloat> Pixelf32;
        typedef Pixelf32 Pixel;


        struct FrameBuffer {
            GLuint texture_id;

            GLuint width;
            GLuint height;
            std::vector<Pixel> data;

            Pixel& at(const GLuint& x, const GLuint& y) {
                assert(x < width);
                assert(y < height);
                return data[x + y * width];
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

        struct Ray {
            glm::vec3 origin;
            glm::vec3 direction; // must be a unit vector
            
            glm::vec3 at(GLfloat distance) const {
                return origin + (direction * distance);
            }
        };

        struct Material {
            Pixel color = { 1.f, 1.f, 1.f };

            float relfectivity = 0.f;

            float opacity = 1.0f;
            float diffraction = 0.f;

            void imgui_panel() {
                if (ImGui::TreeNode("Material")) {
                    ImGui::ColorEdit3("Color", (float*)&color, 0.01f);
                    ImGui::DragFloat("Relfectivity", &relfectivity, 0.01f, 0.f, 1.f);
                    ImGui::DragFloat("Opacity", &opacity, 0.01f, 0.f, 1.f);
                    ImGui::DragFloat("Diffraction", &diffraction, 0.01f, 0.f, 1.f);
                    ImGui::Spacing();

                    ImGui::TreePop();
                }
            }
        };

        struct Sphere {
            glm::vec3 position = { 0.f, 0.f, 0.f };
            float r = 1.f;

            Material material;

            GLfloat intersects(const Ray& ray) const {
                // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
                const glm::vec3& C = position;
                const glm::vec3& O = ray.origin;
                const glm::vec3& D = ray.direction;

                // is behind
                glm::vec3 L = C - O;
                float t_ca = glm::dot(L, D);

                if (t_ca < 0) return f32inf;

                // does miss
                float d2 = glm::dot(L, L) - (t_ca * t_ca);
                d2 = glm::abs(d2);
                if (d2 > r * r) return f32inf;

                float t_hc = sqrt(r * r - d2);
                float t0 = t_ca - t_hc;
                float t1 = t_ca + t_hc;

                if (t0 > t1) std::swap(t0, t1);
                if (t0 < 0) {
                    // if t0 is negative, let's use t1 instead 
                    t0 = t1; 
                    // both t0 and t1 are negative 
                    if (t0 < 0) return f32inf;
                }

                return t0;
            }

        };

        struct Plane {
            glm::vec3 position;
            glm::vec3 normal;
            Material material;

            GLfloat intersects(const Ray& ray) const {
                // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
                float denom = glm::dot(normal, ray.direction);
                if (glm::abs(denom) > 1e-6) {
                    glm::vec3 p0l0 = position - ray.origin;
                    float t = glm::dot(p0l0, normal) / denom;
                    //return t;
                    return t >= 0 ? t : f32inf;
                }
                return f32inf;
            }
        };

        struct Light {
            glm::vec3 position;
            Pixel color;
            float intensity;
        };

        struct Scene {
            const FirstPersonCamera& cam;
            std::vector<Sphere> spheres;
            std::vector<Plane> planes;
            std::vector<Light> lights;

            Scene(const FirstPersonCamera& camera): cam(camera) {
                spheres.push_back({ {0.f, 0.f, -1.f}, 1.f });
                spheres.push_back({ {1.f, 1.f, 1.f}, 0.3f, {0.f, 0.f, 1.f} });
                spheres.push_back({ {1.f, 5.f, 3.f}, 0.8f, {0.f, 1.f, 1.f} });
                spheres.push_back({ {-3.f, 1.f, 1.f}, 1.3f, {1.f, 0.f, 1.f} });
                spheres.push_back({ {2.f, -1.f, -1.f}, 0.1f, {0.f, 1.f, 0.f} });

                planes.push_back({ {0.f, -1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.25f, 0.f} });

                lights.push_back({ { 0.f, 10.f, 0.f }, { 1.f, 1.f, 1.f }, 1.f });
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
                    for (Light& l : lights) {
                        ImGui::PushID(i++);
                        ImGui::DragFloat3("Position", glm::value_ptr(l.position), 0.01f);
                        ImGui::ColorEdit3("Color", (float*)&l.color, 0.01f);
                        ImGui::DragFloat("Intensity", &l.intensity, 0.01f, 0.1f);
                        ImGui::PopID();

                        ImGui::Spacing();
                    }
                    ImGui::TreePop();
                }

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
    
        inline std::tuple<GLfloat, Material, glm::vec3> closest_collision(const Ray& ray, const Scene& scene) {
            GLfloat closest_distance = f32inf;
            Material closest_material = { {0.f, 0.f, 0.f} };
            glm::vec3 normal;

            for (const Sphere& s : scene.spheres) {
                GLfloat current_distance = s.intersects(ray);
                if (current_distance < closest_distance) {
                    closest_material = s.material;
                    closest_distance = current_distance;
                    normal = glm::normalize(ray.at(current_distance) - s.position);
                }
            }

            for (const Plane& p : scene.planes) {
                GLfloat current_distance = p.intersects(ray);
                if (current_distance < closest_distance) {
                    closest_material = p.material;
                    closest_distance = current_distance;
                    normal = p.normal;
                }
            }

            closest_distance = closest_distance < 1e-9 ? f32inf : closest_distance;

            return { closest_distance, closest_material, normal };
        }

        inline void kernel(std::vector<Pixel>& pixels, const int& x, const int& y, const GLuint& w, const GLuint& h, const Scene& scene) {
            GLuint index = x + y * w;
            assert(index < pixels.size());

            Ray ray = calculate_vieport_ray(scene.cam, w, h, x, y);

            auto [distance, material, normal] = closest_collision(ray, scene);

            glm::vec3 pixel_position = ray.at(distance);
            if (distance != f32inf) {

                if (material.relfectivity == 0.f) {
                    float intersections = 0.f;
                    for (const Light& l : scene.lights) {
                        Ray r = { pixel_position, glm::normalize(l.position - ray.at(distance)) };
                        auto [d, m, n] = closest_collision(r, scene);
                        if (d != f32inf) intersections += 1.f;
                    }
                    pixels[index] = material.color * (1.f / (1.f + intersections));
                }
                else {
                    Ray r = { pixel_position, ray.direction - normal * 2.f * glm::dot(ray.direction, normal) };
                    auto [d, m, n] = closest_collision(r, scene);

                    float complement = 1.f - material.relfectivity;

                    pixels[index] = (material.color * complement) + (m.color * material.relfectivity);
                }

            }
            else {
                pixels[index] = material.color;
            }

        }

        void render(FrameBuffer& buffer, GLuint w, GLuint h, const Scene& scene) {
            if (w != buffer.width || h != buffer.height)
                buffer.allocate(w, h);

            #pragma omp parallel for
            for (int x = 0; x < w; ++x)
                for (int y = 0; y < h; ++y) {
                    kernel(buffer.data, x, y, w, h, scene);
                }

            buffer.update();
        }

        void run() {
            /* Create a windowed mode window and its OpenGL context */
            CameraWindow camera_window("RT Spheres");
            Scene scene(camera_window.camera);

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
                scene.imgui_panel();

                // RT settings and rendering
                static float factor = 4;
                {
                    ImGui::Begin("RT Settings");
                    ImGui::DragFloat("Decrease resolution", &factor, 0.01f, 0.5f, 100.f);
                    factor = factor < 0.5f ? 0.5f : factor;
                    ImGui::End();
                }

                GLuint w = camera_window.window.width;
                GLuint h = camera_window.window.height;

                /* update texture */
                render(buffer, w / factor, h / factor, scene);

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
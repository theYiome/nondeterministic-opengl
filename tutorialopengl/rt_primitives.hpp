#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr GLfloat f32inf = std::numeric_limits<GLfloat>::infinity();

template <typename T>
struct PixelTemplate {
    GLfloat r;
    GLfloat g;
    GLfloat b;

    PixelTemplate operator*(const float& m) const {
        return { r * m, g * m, b * m };
    }

    PixelTemplate operator+(const PixelTemplate& pixel2) const {
        return { r + pixel2.r, g + pixel2.g, b + pixel2.b };
    }

    PixelTemplate operator-(const PixelTemplate& pixel2) const {
        return { r - pixel2.r, g - pixel2.g, b - pixel2.b };
    }

    PixelTemplate operator*(const PixelTemplate& pixel2) const {
        return { r * pixel2.r, g * pixel2.g, b * pixel2.b };
    }
};
typedef PixelTemplate<GLfloat> Pixelf32;
typedef Pixelf32 Pixel;

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction; // must be a unit vector

    glm::vec3 at(GLfloat distance) const {
        return origin + (direction * distance);
    }
};

struct Material {
    Pixel color = { 1.f, 1.f, 1.f };

    float emissivity = 0.f;
    float relfectivity = 0.0f;

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

    GLfloat intersects(const Ray& ray) const {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
        float r = 0.618f;
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
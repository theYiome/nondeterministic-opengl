#pragma once
#include <string>

#include <stb_image.h>
#include <GLAD/glad.h>

struct Texture {
    GLuint id = 0;
    int    width = 0;
    int    height = 0;
    int    channels = 0;
    std::vector<unsigned char> data;

    static Texture load_from_path(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        Texture tex;
        glGenTextures(1, &tex.id);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        unsigned char* raw_data = stbi_load(path.c_str(), &tex.width, &tex.height, &tex.channels, 0);
        const long int data_size = tex.width * tex.height * tex.channels;
        tex.data.assign(raw_data, raw_data + data_size);
        stbi_image_free(raw_data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.data.data());
        glGenerateTextureMipmap(tex.id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        return tex;
    }
};
#pragma once
#include <string>

#include <stb_image.h>
#include <GLAD/glad.h>

struct BaseTexture {
    GLuint id = 0;
    int    width = 0;
    int    height = 0;
    int    channels = 0;
};

struct Texture: public BaseTexture {
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

        GLenum color_mode = GL_RGB;
        if (tex.channels == 4)
            color_mode = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, color_mode, GL_UNSIGNED_BYTE, tex.data.data());
        glGenerateTextureMipmap(tex.id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        return tex;
    }
};

struct CubeTexture: public BaseTexture {
    // right, left, top, bottom, back, front
    std::vector<unsigned char> data[6];

    static CubeTexture load_from_path(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        CubeTexture tex;
        glGenTextures(1, &tex.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex.id);
        
        unsigned char* raw_data = stbi_load(path.c_str(), &tex.width, &tex.height, &tex.channels, 0);
        const long int data_size = tex.width * tex.height * tex.channels;
        for (int i = 0; i < 6; ++i)
            tex.data[i].assign(raw_data, raw_data + data_size);
        stbi_image_free(raw_data);

        GLenum color_mode = GL_RGB;
        if (tex.channels == 4)
            color_mode = GL_RGBA;

        for (int i = 0; i < 6; ++i) 
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tex.width, tex.height, 0, color_mode, GL_UNSIGNED_BYTE, tex.data[i].data());

        glGenerateTextureMipmap(tex.id);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return tex;
    }
};
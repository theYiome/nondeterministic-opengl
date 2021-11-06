#include <string>
#include <fstream>
#include <iostream>

std::string read_file(const std::string& filename) {
    std::string line, text;
    std::ifstream file;
    file.open(filename);

    if (file.is_open() == false) {
        std::cout << "Failed to open " << filename << std::endl;
        return "";
    }

    while (std::getline(file, line)) {
        text += line + "\n";
    }

    return text;
}

std::string dump_file_to_string(const std::string& file_path) {
    return read_file(file_path);
}

std::string dump_file_to_string(const char* file_path) {
    return read_file(std::string(file_path));
}

GLuint create_shader_program(const std::string &vertex_shader_path, const std::string &fragment_shader_path) {
    // vertex shader
    std::string vertex_shader_str = dump_file_to_string(vertex_shader_path);

    const char* vertex_shader_c_str = vertex_shader_str.c_str();
    //printf("%s\n", vertex_shader_c_str);

    GLint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_c_str, NULL);
    glCompileShader(vertex_shader);

    // fragment shader
    std::string fragment_shader_str = dump_file_to_string(fragment_shader_path);

    const char* fragment_shader_c_str = fragment_shader_str.c_str();
    //printf("%s\n", fragment_shader_c_str);

    GLint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_c_str, NULL);
    glCompileShader(fragment_shader);

    // shader program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return shader_program;
}

#include <chrono>

void printFPS() {
    // c++11 and greater
    #if __cplusplus >= 201103L
        static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
        static int fps; fps++;

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - oldTime) >= std::chrono::seconds{ 1 }) {
            oldTime = std::chrono::high_resolution_clock::now();
            std::cout << "FPS: " << fps << std::endl;
            fps = 0;
        }
    #endif
}
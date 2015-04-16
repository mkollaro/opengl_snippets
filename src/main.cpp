#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#ifndef SHADERS_DIR
#define SHADERS_DIR "./shaders/"
#endif

const float vertices[] = {
     0.0f,  0.5f,
     0.5f, -0.5f,
    -0.5f, -0.5f
};

void keyPressHandler(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) return;

}

const std::string readFile(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "Unable to open file " << filepath << std::endl;
        throw std::runtime_error("I/O Error");
    }
    std::string str((std::istreambuf_iterator<char>(f)),
                     std::istreambuf_iterator<char>());
    return str;
}

enum class ShaderType {vertex, fragment};

/**
 * Read |filename| from SHADERS_DIR, compile it as a shader and return its ID.
 */
GLuint compileShader(const std::string& filename, ShaderType type) {
    auto source = readFile(SHADERS_DIR + filename).c_str();
    auto gl_type = GL_VERTEX_SHADER;
    if (type != ShaderType::vertex) gl_type = GL_FRAGMENT_SHADER;

    GLuint shader = glCreateShader(gl_type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        std::cerr << "Failed to compile shader:\n";
        char buffer[512];
        std::cerr << source << std::endl;
        std::cerr << "************************************\n";
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        std::cerr << buffer;
        throw std::exception();
    }
    return shader;
}

/**
 * Return ID of the linked and activated shader.
 */
GLuint initShaders() {
    auto vshader = compileShader("vshader.glsl", ShaderType::vertex);
    auto fshader = compileShader("fshader.glsl", ShaderType::fragment);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vshader);
    glAttachShader(shaderProgram, fshader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    return shaderProgram;
}

SDL_GLContext initContext(SDL_Window *window) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    return context;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Hello World",
                                           100, 100, 800, 600,
                                           SDL_WINDOW_OPENGL);
    auto context = initContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
            else keyPressHandler(event);
        }
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

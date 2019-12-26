#ifndef _LEARN_GL_H_
#define _LEARN_GL_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>

static void
checkCompileErrors(unsigned int shader, std::string type)
{
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
        }
        else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
        }
}

static uint32_t
create_shader(const char* path, int32_t shader_type)
{
        std::string code;
        std::ifstream shader_file;
        shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            shader_file.open(path);
            std::stringstream vShaderStream;

            // read file's buffer contents into streams
            vShaderStream << shader_file.rdbuf();

            // close file handlers
            shader_file.close();

            // convert stream into string
            code = vShaderStream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const char* code_c = code.c_str();

        uint32_t shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &code_c, NULL);
        glCompileShader(shader);
        checkCompileErrors(shader, path);

        return shader;
}

static uint32_t
create_shader_program(uint32_t vertex, uint32_t fragment)
{
        uint32_t shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertex);
        glAttachShader(shaderProgram, fragment);
        glLinkProgram(shaderProgram);
        checkCompileErrors(shaderProgram, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return shaderProgram;
}

static void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLFWwindow*
init_gl(uint32_t scrwidth, uint32_t scrheight)
{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(scrwidth,
                                              scrheight,
                                              "LearnOpenGL",
                                              NULL,
                                              NULL);
        if (window == NULL) {
                fprintf(stderr, "%s\n", "Failed to create GLFW window.");
                glfwTerminate();
                return NULL;
        }
        glfwMakeContextCurrent(window);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                fprintf(stderr, "%s\n", "Failed to initialize GLAD");
                return NULL;
        }

        return window;
}

static uint32_t
init_VAO(float *vertices, size_t verts_sz)
{
        uint32_t VAO;
        unsigned int VBO;
        unsigned int EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     verts_sz,
                     vertices,
                     GL_STATIC_DRAW);

        return VAO;
}

static void
init_vert_attr(uint32_t index, int32_t size, size_t stride, size_t offset)
{
        glVertexAttribPointer(index,
                              size,
                              GL_FLOAT,
                              GL_FALSE,
                              stride * sizeof(float),
                              (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
}

#endif /* _LEARN_GL_H_ */
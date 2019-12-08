#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

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
process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
}

static void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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

int main(void)
{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
        if (window == NULL) {
                fprintf(stderr, "%s\n", "Failed to create GLFW window.");
                glfwTerminate();
                return -1;
        }
        glfwMakeContextCurrent(window);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                fprintf(stderr, "%s\n", "Failed to initialize GLAD");
                return -1;
        }

        uint32_t vertex = create_shader("./shader.vs", GL_VERTEX_SHADER);
        uint32_t fragment = create_shader("./shader.fs", GL_FRAGMENT_SHADER);
        uint32_t shader_prog = create_shader_program(vertex, fragment);

        float vertices[] = {
                0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
                0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
        };
        unsigned int indices[] = {
                0, 1, 2,  // first Triangle
        };
        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(vertices),
                     vertices,
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(indices),
                     indices,
                     GL_STATIC_DRAW);

        init_vert_attr(0, 3, 6, 0);
        init_vert_attr(1, 3, 6, 3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        while (!glfwWindowShouldClose(window)) {
                process_input(window);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(shader_prog);

                int horiz_offs = glGetUniformLocation(shader_prog,
                                                      "horiz_offs");
                glUniform1f(horiz_offs, 0.5f);

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES,
                               sizeof(indices)/sizeof(indices[0]),
                               GL_UNSIGNED_INT,
                               0);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();

        return EXIT_SUCCESS;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define UP 1
#define NEUTRAL 0
#define DOWN -1

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

static uint32_t
init_VAO(uint32_t *indices, size_t inds_sz, float *vertices, size_t verts_sz)
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

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     inds_sz,
                     indices,
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

static float
process_input(GLFWwindow *window,
              uint32_t shader_prog,
              float alpha,
              int32_t& prev_state)
{
        constexpr float amt = 0.1f;
        int32_t uni_loc;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) &&
            (prev_state != UP)) {
                alpha = fmin(alpha + amt, 1.0f);
                prev_state = UP;

                uni_loc = glGetUniformLocation(shader_prog, "alpha");
                glUniform1f(uni_loc, alpha);
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (prev_state != DOWN)) {
                alpha = fmax(alpha - amt, 0.0f);
                prev_state = DOWN;

                uni_loc = glGetUniformLocation(shader_prog, "alpha");
                glUniform1f(uni_loc, alpha);
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS) &&
            (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS) &&
            (prev_state != NEUTRAL))
                prev_state = NEUTRAL;

        return alpha;
}

static void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLFWwindow*
init_gl(void)
{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
gen_texture(const char* path, int32_t format)
{
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             format,
                             width,
                             height,
                             0,
                             format,
                             GL_UNSIGNED_BYTE,
                             data);
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
                std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        return texture;
}

int main(void)
{
        GLFWwindow* window = init_gl();
        if (window == NULL)
                return -1;

        uint32_t vertex = create_shader("./shader.vs", GL_VERTEX_SHADER);
        uint32_t fragment = create_shader("./shader.fs", GL_FRAGMENT_SHADER);
        uint32_t shader_prog = create_shader_program(vertex, fragment);

        float vertices[] = {
                // positions          // colors           // texture coords
                0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
                0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
                -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
                -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
        };
        unsigned int indices[] = {  
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };
        uint32_t VAO = init_VAO(indices,
                                sizeof(indices),
                                vertices,
                                sizeof(vertices));

        init_vert_attr(0, 3, 8, 0);
        init_vert_attr(1, 3, 8, 3);
        init_vert_attr(2, 2, 8, 6);

        stbi_set_flip_vertically_on_load(true);
        uint32_t texture1 = gen_texture("/home/bduke/work/LearnOpenGL/resources/textures/container.jpg",
                                        GL_RGB);
        uint32_t texture2 = gen_texture("/home/bduke/work/LearnOpenGL/resources/textures/awesomeface.png",
                                        GL_RGBA);

        glUseProgram(shader_prog);
        glUniform1i(glGetUniformLocation(shader_prog, "texture1"), 0);
        glUniform1i(glGetUniformLocation(shader_prog, "texture2"), 1);

        float alpha = 0.5f;
        int32_t uni_loc = glGetUniformLocation(shader_prog, "alpha");
        glUniform1f(uni_loc, alpha);
        int32_t prev_state = NEUTRAL;
        while (!glfwWindowShouldClose(window)) {
                alpha = process_input(window, shader_prog, alpha, prev_state);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture2);

                glUseProgram(shader_prog);

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

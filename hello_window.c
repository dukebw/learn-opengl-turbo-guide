#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define UNUSED(x) x __attribute__ ((__unused__))

static void
bind_buffer(uint32_t buffer,
            GLenum target,
            const void *data,
            uint32_t data_size_bytes)
{
        glBindBuffer(target, buffer);
        glBufferData(target, data_size_bytes, data, GL_STATIC_DRAW);
}

static void
error_callback(int32_t error, const char *description)
{
        fprintf(stderr, "Error: %d. %s\n", error, description);
}

static void
framebuffer_size_callback(GLFWwindow *UNUSED(window),
                          int32_t width,
                          int32_t height)
{
        glViewport(0, 0, width, height);
}

static void
process_input(GLFWwindow *window)
{
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static uint32_t
create_compile_shader(const char **shader_src, GLenum shader_type)
{
        uint32_t shader = glCreateShader(shader_type);
        assert(shader != 0);

        glShaderSource(shader, 1, shader_src, NULL);
        glCompileShader(shader);

        int32_t success;
        char info_log[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
                glGetShaderInfoLog(shader,
                                   sizeof(info_log),
                                   NULL,
                                   info_log);
                printf("Error! Shader compilation failed\n%s\n", info_log);
                exit(EXIT_FAILURE);
        }

        return shader;
}

int main(void)
{
        assert(glfwInit() == GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwSetErrorCallback(error_callback);

        GLFWwindow *window = glfwCreateWindow(800,
                                              600,
                                              "Learn OpenGL",
                                              NULL,
                                              NULL);
        assert(window != NULL);

        glfwMakeContextCurrent(window);

        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);

        glViewport(0, 0, 800, 600);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        const char *vertex_shader_src =
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;\n"
                ""
                "void main()\n"
                "{\n"
                "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                "}\n";
        const char *fragment_shader_src =
                "#version 330 core\n"
                "out vec4 FragColor;\n"
                ""
                "void main()\n"
                "{\n"
                "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                "}\n";

        uint32_t vertex_shader = create_compile_shader(&vertex_shader_src,
                                                       GL_VERTEX_SHADER);
        uint32_t fragment_shader = create_compile_shader(&fragment_shader_src,
                                                         GL_FRAGMENT_SHADER);

        uint32_t shader_program = glCreateProgram();
        assert(shader_program != 0);

        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        int32_t success;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
                char info_log[512];
                glGetProgramInfoLog(shader_program,
                                    sizeof(info_log),
                                    NULL,
                                    info_log);
                printf("Shader program linking failed!\n%s\n", info_log);
                exit(EXIT_FAILURE);
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        float vertices[] = {
                0.5f, 0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f
        };
        uint32_t indices[] = {
                0, 1, 3,
                1, 2, 3
        };

        uint32_t EBO;
        glGenBuffers(1, &EBO);

        uint32_t VBO;
        glGenBuffers(1, &VBO);

        uint32_t VAO;
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        bind_buffer(EBO, GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));
        bind_buffer(VBO, GL_ARRAY_BUFFER, vertices, sizeof(vertices));

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
        glEnableVertexAttribArray(0);

        for (uint32_t counter = 0;
             !glfwWindowShouldClose(window);
             ++counter) {
                if ((counter % 120) == 0) {
                        int32_t polygon_mode;
                        glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);

                        if (polygon_mode == GL_LINE)
                                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        else if (polygon_mode == GL_FILL)
                                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }

                process_input(window);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(shader_program);
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        return EXIT_SUCCESS;
}
